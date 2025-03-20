#include <Arduino.h>
#include "queue.h"

#define MAZE_DIM 16
#define MAZE_SIZE MAZE_DIM*MAZE_DIM

// Maze representation: 16x16 unit grid
byte walls[MAZE_SIZE]; // walls[i][j] = [WSEN] means cell (i,j) has walls around it according to bitmap
byte path_cost[MAZE_DIM][MAZE_DIM]; // path_cost[i][j] = k means cell (i,j) has a path cost of k to the 4 center cells

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define CLEAR 255 // Indicates a cell has no path cost
// Sensor directions
const int FORWARD = 0;
const int LEFT = 1;
const int RIGHT = 2;


int check_bit(byte num, int n) {
  return ((num & (1 << n)) != 0);
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

int opposite_dir(int dir) {
  switch (dir) {
    case NORTH: return SOUTH;
    case EAST: return WEST;
    case SOUTH: return NORTH;
    case WEST: return EAST;
  }
}

int bordering_cell(int cell, int dir) {
  int row = cell / MAZE_DIM;
  int col = cell % MAZE_DIM;
  switch (dir) {
    case NORTH: return ((row > 0) ? (cell - MAZE_DIM) : cell);
    case EAST: return ((col < (MAZE_DIM - 1)) ? (cell + 1) : cell);
    case SOUTH: return ((row < (MAZE_DIM - 1)) ? (cell + MAZE_DIM) : cell);
    case WEST: return ((col > 0) ? (cell - 1) : cell);
  }
}

// Add detected wall to representation of maze layout
void update_layout(int cell, int dir) {
  if (!check_bit(walls[cell], dir)) {
    walls[cell] += wall_decode(dir);
    // Update bordering cell
    int other_cell = bordering_cell(cell, dir);
    if (other_cell != cell) walls[other_cell] += wall_decode(opposite_dir(dir));
  }
}

// Figure out where in maze the detected wall is
void wall_int(int current_cell, int dir, int sensor_dir) {
  if (sensor_dir == FORWARD) {
    switch (dir) {
      case NORTH: update_layout(current_cell, NORTH); break;
      case EAST: update_layout(current_cell, EAST); break;
      case SOUTH: update_layout(current_cell, SOUTH); break;
      case WEST: update_layout(current_cell, WEST); break;
    }
  }
  else if (sensor_dir == LEFT) {
    switch (dir) {
      case NORTH: update_layout(current_cell, WEST); break;
      case EAST: update_layout(current_cell, NORTH); break;
      case SOUTH: update_layout(current_cell, EAST); break;
      case WEST: update_layout(current_cell, SOUTH); break;
    }
  }
  else if (sensor_dir == RIGHT) {
    switch (dir) {
      case NORTH: update_layout(current_cell, EAST); break;
      case EAST: update_layout(current_cell, SOUTH); break;
      case SOUTH: update_layout(current_cell, WEST); break;
      case WEST: update_layout(current_cell, NORTH); break;
    }
  }
}

void print_walls() {
  Serial.print(F("WALLS:"));
  for (int i = 0; i < MAZE_DIM; i++) {
    Serial.println();
    for (int j = 0; j < MAZE_DIM; j++) {
      Serial.print(walls[i*MAZE_DIM + j]);
      Serial.print(F(" "));
    }
  }
  Serial.println();
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
  /*
  Serial.print(F("Cell: "));
  Serial.print(row);
  Serial.print(F(" "));
  Serial.print(col);
  Serial.print(F(" no wall UP: "));
  Serial.print(!check_bit(walls[current_cell], NORTH));
  Serial.print(F(" no wall DOWN: "));
  Serial.print(!check_bit(walls[current_cell], SOUTH));
  Serial.print(F(" no wall LEFT: "));
  Serial.print(!check_bit(walls[current_cell], WEST));
  Serial.print(F(" no wall RIGHT: "));
  Serial.print(!check_bit(walls[current_cell], EAST));
  Serial.println();
  */

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
  //print_costs();
}