#include <Arduino.h>
#include "floodfill.h"
#include "sensors.h"
#include "movement.h"

int curr_dir; // which way are we facing with respect to center
// Sensor directions
const int FORWARD = 0;
const int LEFT = 1;
const int RIGHT = 2;

void reorient(int dir) {
  if (dir == NORTH) {
    switch(curr_dir) {
      case NORTH: break;
      case EAST: move(PD.LEFT, 1); break;
      case SOUTH: move(PD.LEFT, 2); break;
      case WEST: move(PD.RIGHT, 1); break;
    }
  }
  if (dir == EAST) {
    switch(curr_dir) {
      case NORTH: move(PD.RIGHT, 1); break;
      case EAST: break;
      case SOUTH: move(PD.LEFT, 1); break;
      case WEST: move(PD.RIGHT, 2); break;
    }
  }
  if (dir == SOUTH) {
    switch(curr_dir) {
      case NORTH: move(PD.LEFT, 2); break;
      case EAST: move(PD.RIGHT, 1); break;
      case SOUTH: break;
      case WEST: move(PD.LEFT, 1); break;
    }
  }
  if (dir == WEST) {
    switch(curr_dir) {
      case NORTH: move(PD.LEFT, 1); break;
      case EAST: move(PD.LEFT, 2); break;
      case SOUTH: move(PD.RIGHT, 1); break;
      case WEST: break;
    }
  }
}

int decode_sensors(int sensor_num) {
}

void clear_path(int path[MAZE_SIZE]) {
  for (int i = 0; i < MAZE_SIZE; i++) {
    path[i] = -1;
  }
}

// Add detected wall to representation of maze layout
void update_layout(int cell, int dir) {
  if (!check_bit(walls[cell], dir)) walls[cell] += wall_decode(dir);
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

int in_path(int cell, int path[MAZE_SIZE]) {
  for (int i = 0; i < MAZE_SIZE; i++) {
    if (path[i] == cell) return 1;
  }
  return 0;
}

void print_path(/*int path_cost[MAZE_DIM][MAZE_DIM],*/ int path[MAZE_SIZE]) {
  for (int i = 0; i < 5; i++) Serial.println();
  int cell;
  for (int i = 0; i < MAZE_DIM; i++) {
    for (int j = 0; j < MAZE_DIM; j++) {
      cell = i*MAZE_DIM + j;
      if (in_path(cell, path)) {
        Serial.print("*  ");
      }
      else {
        Serial.print(path_cost[i][j]);
        Serial.print(" ");
        if (path_cost[i][j] <= 9) Serial.print(" "); // To line up w/ 2 digit nums
      }
    }
    Serial.println();
  }
}

int center_reached(int current_cell) {
  return (
    (current_cell == ((MAZE_DIM/2 - 1)*MAZE_DIM + (MAZE_DIM/2 - 1))) || 
    (current_cell == ((MAZE_DIM/2 - 1)*MAZE_DIM + (MAZE_DIM/2))) || 
    (current_cell == ((MAZE_DIM/2)*MAZE_DIM + (MAZE_DIM/2 - 1))) || 
    (current_cell == ((MAZE_DIM/2)*MAZE_DIM + (MAZE_DIM/2)))
  );
}

// Find adjacent cell with lowest path cost
int min_i(int row, int col) {
  int min_c = MAZE_SIZE;
  int min_cell = row*MAZE_DIM + col;

  // Check cell below
  if (row < (MAZE_DIM - 1)) {
    if (path_cost[row + 1][col] < min_c) {
      min_c = path_cost[row + 1][col];
      min_cell = SOUTH;
    }
  }
  // Check cell to left
  if (col > 0) {
    if (path_cost[row][col - 1] < min_c) {
      min_c = path_cost[row][col - 1];
      min_cell = WEST;
    }
  }
  // Check cell above
  if (row > 0) {
    if (path_cost[row - 1][col] < min_c) {
      min_c = path_cost[row - 1][col];
      min_cell = NORTH;
    }
  }
  // Check cell to right
  if (col < (MAZE_DIM - 1)) {
    if (path_cost[row][col + 1] < min_c) {
      min_c = path_cost[row][col + 1];
      min_cell = EAST;
    }
  }
  return min_cell;
}

// Continuously move to adjacent cell with lowest path cost until wall is detected or center is reached
int traverse(int current_cell, int* i, int path[MAZE_SIZE]) {
  int row = current_cell / MAZE_DIM;
  int col = current_cell % MAZE_DIM;

  while (!check_bit(walls[current_cell], min_i(row, col))) {
    path[*i] = current_cell;
    *i = *i + 1;

    int dir = min_i(row, col);
    Serial.print("Current cell: ");
    Serial.print(row);
    Serial.print(" ");
    Serial.print(col);
    Serial.print(" , Facing: ");
    Serial.print(dir);
    reorient(dir); // Face this direction
    switch (dir) {
      case NORTH: current_cell = (row-1)*MAZE_DIM + col; break;
      case EAST: current_cell = row*MAZE_DIM + (col + 1); break;
      case SOUTH: current_cell = (row+1)*MAZE_DIM + col; break;
      case WEST: current_cell = row*MAZE_DIM + (col - 1); break;
    }
    row = current_cell / MAZE_DIM;
    col = current_cell % MAZE_DIM;
    Serial.print(" , Moving to cell ");
    Serial.print(row);
    Serial.print(" ");
    Serial.println(col);
    move(PD.FORWARD, 1); // Move to next cell

    if (center_reached(current_cell)) return current_cell;
  }
  return current_cell;
}

void run() {
  int path[MAZE_SIZE]; // save path traversed
  clear_path(path);
  init_layout();
  curr_dir = SOUTH;
  int current_cell = 0; // start at top left
  int i = 0; // tracks path length

  while(!center_reached(current_cell)) {
    floodfill();
    current_cell = traverse(current_cell, &i, path);
    print_path(path);
  }
}