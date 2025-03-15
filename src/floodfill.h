#include <Arduino.h>
#include "queue.h"

#define MAZE_DIM 4
#define MAZE_SIZE MAZE_DIM*MAZE_DIM

// Maze representation: 16x16 unit grid
int walls[MAZE_SIZE]; // walls[i][j] = NSEW means cell (i,j) has walls around it according to bitmap
int path_cost[MAZE_DIM][MAZE_DIM]; // path_cost[i][j] = k means cell (i,j) has a path cost of k to the 4 center cells

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define CLEAR 256 // Indicates a cell has no path cost

int check_bit(int num, int n) {
  return (num & (1 << n)) != 0;
}

int wall_decode(int dir) {
  int add_val;
  switch(dir) {
    case NORTH: add_val = 1; break;
    case EAST: add_val = 2; break;
    case SOUTH: add_val = 4; break;
    case WEST: add_val = 8; break;
    default: add_val = 0; break;
  }
  return add_val;
}

// initially assume there are no walls in the maze
void init_layout() {
  for (int i = 0; i < MAZE_SIZE; i++) {
    walls[i] = 0;
  }
}

// set all path costs to CLEAR (not yet set)
void clear_costs() {
  for (int i = 0; i < MAZE_DIM; i++) {
    for (int j = 0; j < MAZE_DIM; j++) {
      path_cost[i][j] = CLEAR; // 0 used for center
    }
  }
}

void print_costs() {
  for (int i = 0; i < MAZE_DIM; i++) {
    for (int j = 0; j < MAZE_DIM; j++) {
      Serial.print(path_cost[i][j]);
      Serial.print(" ");
      if (path_cost[i][j] <= 9) Serial.print(" ");
    }
    Serial.println();
  }
}

void check_neighbors(int current_cell) {
  int row = current_cell / MAZE_DIM;
  int col = current_cell % MAZE_DIM;
  
  // Check each direction
  // Down
  if ( (path_cost[row + 1][col] == CLEAR) && !(check_bit(walls[current_cell], SOUTH)) ) {
    path_cost[row + 1][col] = path_cost[row][col] + 1;
    q.enqueue((row+1)*MAZE_DIM + col);
  }
  // Right
  if ( (path_cost[row][col + 1] == CLEAR) && !(check_bit(walls[current_cell], EAST)) ) {
    path_cost[row][col + 1] = path_cost[row][col] + 1;
    q.enqueue(row*MAZE_DIM + (col + 1));
  }
  // Up
  if ( (path_cost[row - 1][col] == CLEAR) && !(check_bit(walls[current_cell], NORTH)) ) {
    path_cost[row - 1][col] = path_cost[row][col] + 1;
    q.enqueue((row-1)*MAZE_DIM + col);
  }
  // Left
  if ( (path_cost[row ][col - 1] == CLEAR) && !(check_bit(walls[current_cell], WEST)) ) {
    path_cost[row][col - 1] = path_cost[row][col] + 1;
    q.enqueue(row*MAZE_DIM + (col - 1));
  }
  return;
}

void floodfill() {
  // S0
  clear_costs();
  // Set path cost of center cells to 0
  path_cost[MAZE_DIM/2 - 1][MAZE_DIM/2 - 1] = 0;
  path_cost[MAZE_DIM/2 - 1][MAZE_DIM/2] = 0;
  path_cost[MAZE_DIM/2][MAZE_DIM/2 - 1] = 0;
  path_cost[MAZE_DIM/2][MAZE_DIM/2] = 0;
  
  // Cell (row,col) given a number n is (n/16, n%16) ; n = row*16 + col
  q.enqueue((MAZE_DIM/2 - 1)*MAZE_DIM + (MAZE_DIM/2 - 1));
  q.enqueue((MAZE_DIM/2 - 1)*MAZE_DIM + (MAZE_DIM/2));
  q.enqueue((MAZE_DIM/2)*MAZE_DIM + (MAZE_DIM/2 - 1));
  q.enqueue((MAZE_DIM/2)*MAZE_DIM + (MAZE_DIM/2));

  // S1
  int current_cell;
  do {
    current_cell = q.front();
    q.dequeue();
    check_neighbors(current_cell);
  } while(!q.empty());
  print_costs();
}