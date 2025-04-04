#include <Arduino.h>
#include "queue.h"
#include "wall_detection.h"

// path_cost[i][j] = k means cell (i,j) has a path cost of k to the 4 center cells
byte path_cost[MAZE_DIM][MAZE_DIM];

#define CLEAR 255 // Indicates a cell has no path cost

// set all path costs to CLEAR (not yet set)
void clear_costs() {
  for (int i = 0; i < MAZE_DIM; i++) {
    for (int j = 0; j < MAZE_DIM; j++) {
      path_cost[i][j] = CLEAR; // 0 used for center
    }
  }
}

void print_costs() {
  Serial.print(F("PATH COSTS:"));
  for (int i = 0; i < MAZE_DIM; i++) {
    Serial.println();
    for (int j = 0; j < MAZE_DIM; j++) {
      Serial.print(path_cost[i][j]);
      Serial.print(F(" "));
      if (path_cost[i][j] <= 9) Serial.print(F(" "));
    }
  }
  Serial.println();
}

void check_neighbors(int current_cell) {
  int row = current_cell / MAZE_DIM;
  int col = current_cell % MAZE_DIM;
  // Check each direction
  // Down
  if (row < (MAZE_DIM - 1)) {
    if ( (path_cost[row + 1][col] == CLEAR) && !check_bit(walls[current_cell], SOUTH) ) {
      path_cost[row + 1][col] = path_cost[row][col] + 1;
      q.enqueue((row+1)*MAZE_DIM + col);
    }
  }
  // Right
  if (col < (MAZE_DIM - 1)) {
    if ( (path_cost[row][col + 1] == CLEAR) && !check_bit(walls[current_cell], EAST) ) {
      path_cost[row][col + 1] = path_cost[row][col] + 1;
      q.enqueue(row*MAZE_DIM + (col + 1));
    }
  }
  // Up
  if (row > 0) {
    if ( (path_cost[row - 1][col] == CLEAR) && !check_bit(walls[current_cell], NORTH) ) {
      path_cost[row - 1][col] = path_cost[row][col] + 1;
      q.enqueue((row-1)*MAZE_DIM + col);
    }
  }
  // Left
  if (col > 0) {
    if ( (path_cost[row ][col - 1] == CLEAR) && !check_bit(walls[current_cell], WEST) ) {
      path_cost[row][col - 1] = path_cost[row][col] + 1;
      q.enqueue(row*MAZE_DIM + (col - 1));
    }
  }
  return;
}

void floodfill(int returning) {
  // S0
  clear_costs();
  // Returning to center
  if (returning) {
    path_cost[0][0] = 0;
    q.enqueue(0);
  }
  // Set path cost of center cells to 0
  else {
    path_cost[MAZE_DIM/2 - 1][MAZE_DIM/2 - 1] = 0;
    path_cost[MAZE_DIM/2 - 1][MAZE_DIM/2] = 0;
    path_cost[MAZE_DIM/2][MAZE_DIM/2 - 1] = 0;
    path_cost[MAZE_DIM/2][MAZE_DIM/2] = 0;
    // Cell (row,col) given a number n is (n/16, n%16) ; n = row*16 + col
    q.enqueue((MAZE_DIM/2 - 1)*MAZE_DIM + (MAZE_DIM/2 - 1));
    q.enqueue((MAZE_DIM/2 - 1)*MAZE_DIM + (MAZE_DIM/2));
    q.enqueue((MAZE_DIM/2)*MAZE_DIM + (MAZE_DIM/2 - 1));
    q.enqueue((MAZE_DIM/2)*MAZE_DIM + (MAZE_DIM/2));
  }
  // S1
  int current_cell;
  do {
    current_cell = q.front();
    q.dequeue();
    check_neighbors(current_cell);
  } while(!q.empty());
  //print_costs();
}