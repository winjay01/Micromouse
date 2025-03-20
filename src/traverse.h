#include <Arduino.h>
#include "floodfill.h"
#include "sensors.h"
#include "movement.h"

int curr_dir; // which way are we facing with respect to center

void print_path_cells(byte path[MAZE_SIZE]) {
  int i = 0;
  Serial.println(F("PATH:"));
  do { 
    Serial.print(path[i++]);
    Serial.print(F(" "));
  } while (path[i] != 0);
  Serial.println();
}

int get_next_dir(int cell, int next_cell) {
  if (next_cell == (cell - MAZE_DIM)) return NORTH;
  else if (next_cell == (cell + 1)) return EAST;
  else if (next_cell == (cell + MAZE_DIM)) return SOUTH;
  else if (next_cell == (cell - 1)) return WEST;
}

// Get next straight path
int get_num_cells(byte path[MAZE_SIZE], int cell_num) {
  int init_cell_num = cell_num;
  int cell = path[cell_num];
  int next_cell = path[cell_num + 1];
  int dir = get_next_dir(cell, next_cell);
  int init_dir = dir;
  while ((next_cell != 0) && (init_dir == dir)) {
    cell_num += 1;
    cell = next_cell;
    next_cell = path[cell_num + 1];
    dir = get_next_dir(cell, next_cell);
  } 
  return (cell_num - init_cell_num);
}

void reorient(int dir) {
  if (dir == NORTH) {
    switch(curr_dir) {
      case NORTH: break;
      case EAST: turn(PD.LEFT, 1); break;
      case SOUTH: turn(PD.LEFT, 2); break;
      case WEST: turn(PD.RIGHT, 1); break;
    }
  }
  if (dir == EAST) {
    switch(curr_dir) {
      case NORTH: turn(PD.RIGHT, 1); break;
      case EAST: break;
      case SOUTH: turn(PD.LEFT, 1); break;
      case WEST: turn(PD.RIGHT, 2); break;
    }
  }
  if (dir == SOUTH) {
    switch(curr_dir) {
      case NORTH: turn(PD.LEFT, 2); break;
      case EAST: turn(PD.RIGHT, 1); break;
      case SOUTH: break;
      case WEST: turn(PD.LEFT, 1); break;
    }
  }
  if (dir == WEST) {
    switch(curr_dir) {
      case NORTH: turn(PD.LEFT, 1); break;
      case EAST: turn(PD.LEFT, 2); break;
      case SOUTH: turn(PD.RIGHT, 1); break;
      case WEST: break;
    }
  }
}

int decode_sensors(int sensor_num) {
}

void clear_path(byte path[MAZE_SIZE], int start_from) {
  for (int i = start_from; i < MAZE_SIZE; i++) {
    path[i] = 0;
  }
}

int in_path(int cell, byte path[MAZE_SIZE]) {
  for (int i = 0; i < MAZE_SIZE; i++) {
    if (path[i] == cell) return 1;
  }
  return 0;
}

void print_path(byte path[MAZE_SIZE]) {
  for (int i = 0; i < 2; i++) Serial.println();
  int cell;
  for (int i = 0; i < MAZE_DIM; i++) {
    for (int j = 0; j < MAZE_DIM; j++) {
      cell = i*MAZE_DIM + j;
      if (in_path(cell, path)) {
        Serial.print(F("*  "));
      }
      else {
        Serial.print(path_cost[i][j]);
        Serial.print(F(" "));
        if (path_cost[i][j] <= 9) Serial.print(F(" ")); // To line up w/ 2 digit nums
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
int min_dir(int row, int col) {
  int min_c = MAZE_SIZE;
  int min_d = 5;
  int current_cell = row*MAZE_DIM + col;

  // Check cell below
  if (row < (MAZE_DIM - 1)) {
    if (!check_bit(walls[current_cell], SOUTH) && path_cost[row + 1][col] < min_c) {
      min_c = path_cost[row + 1][col];
      min_d = SOUTH;
    }
  }
  // Check cell to left
  if (col > 0) {
    if (!check_bit(walls[current_cell], WEST) && path_cost[row][col - 1] < min_c) {
      min_c = path_cost[row][col - 1];
      min_d = WEST;
    }
  }
  // Check cell above
  if (row > 0) {
    if (!check_bit(walls[current_cell], NORTH) && path_cost[row - 1][col] < min_c) {
      min_c = path_cost[row - 1][col];
      min_d = NORTH;
    }
  }
  // Check cell to right
  if (col < (MAZE_DIM - 1)) {
    if (!check_bit(walls[current_cell], EAST) && path_cost[row][col + 1] < min_c) {
      min_c = path_cost[row][col + 1];
      min_d = EAST;
    }
  }
  return min_d;
}

// Continuously move to adjacent cell with lowest path cost until wall is detected or center is reached
int find_path(int current_cell, int i, byte path[MAZE_SIZE]) {
  int init_i = i;
  int row = current_cell / MAZE_DIM;
  int col = current_cell % MAZE_DIM;
  path[i++] = current_cell;

  while (!center_reached(current_cell)) {
    int dir = min_dir(row, col);
    switch (dir) {
      case NORTH: current_cell = (row-1)*MAZE_DIM + col; break;
      case EAST: current_cell = row*MAZE_DIM + (col + 1); break;
      case SOUTH: current_cell = (row+1)*MAZE_DIM + col; break;
      case WEST: current_cell = row*MAZE_DIM + (col - 1); break;
      default: Serial.print(F("No dir found, cell: ")); Serial.print(row); Serial.print(F(" ")); Serial.println(col); break;
    }
    if (in_path(current_cell, path))  { // Backtracking 
      clear_path(path, (i - 1));
      print_path_cells(path);
      return (i - 2);
    }
    path[i++] = current_cell;
    row = current_cell / MAZE_DIM;
    col = current_cell % MAZE_DIM;
  }
  print_path_cells(path);
  return init_i;
}

int traverse(int current_cell, int* i, byte path[MAZE_SIZE]) {
  int next_path_length, dir, next_cell, actual_length;
  int backtracking = 0;
  if (!in_path(current_cell, path)) { // Have to backtrack
    next_path_length = 1;
    backtracking = 1;
  }
  else next_path_length = get_num_cells(path, *i);

  int row = current_cell / MAZE_DIM;
  int col = current_cell % MAZE_DIM;
  while (next_path_length > 0) {
    delay(1000);
    if (!backtracking) {
      dir = get_next_dir(current_cell, path[*i + 1]);
      next_cell = path[*i + next_path_length];
    }
    else {
      dir = get_next_dir(current_cell, path[*i]);
      next_cell = path[*i];
    }

    Serial.print(F("Current cell: "));
    Serial.print(row);
    Serial.print(F(" "));
    Serial.print(col);
    Serial.print(F(" , Facing: "));
    Serial.print(dir);
    //reorient(dir); // Face this direction
    curr_dir = dir;

    row = next_cell / MAZE_DIM;
    col = next_cell % MAZE_DIM;
    Serial.print(F(" , Moving to cell "));
    Serial.print(row);
    Serial.print(F(" "));
    Serial.println(col);

    //actual_length = move(PD.FORWARD, next_path_length); // Move to next cell
    if (!backtracking) {
      switch (current_cell) {
        case 0: actual_length = 3; break;
        case 3*MAZE_DIM: actual_length = 0; break;
        default: actual_length = next_path_length; break;
      }
      *i = *i + actual_length;
    }
    else {
      actual_length = 1;
    }
    current_cell = path[*i];
    Serial.print(F("Actually moved to cell "));
    Serial.print(current_cell / MAZE_DIM);
    Serial.print(F(" "));
    Serial.println(current_cell % MAZE_DIM);
    
    if (actual_length < next_path_length) { // Find new path
      wall_int(current_cell, curr_dir, FORWARD); // Update layout
      clear_path(path, (*i + 1));
      return current_cell;
    }
    next_path_length = get_num_cells(path, *i);
  }
  return current_cell;
}

void run() {
  byte path[MAZE_SIZE]; // save path traversed
  clear_path(path, 0);
  init_layout();
  curr_dir = SOUTH; // start facing right w/ respect center, no matter which corner robot is dropped in
  int current_cell = 0; // start at top left
  int i = 0; // tracks path length

  while(!center_reached(current_cell)) {
    floodfill();
    i = find_path(current_cell, i, path);
    //Serial.print(F("i = "));
    //Serial.println(i);
    print_path(path);
    current_cell = traverse(current_cell, &i, path);
    //print_walls();
  }
  Serial.println(F("Center reached!"));
}