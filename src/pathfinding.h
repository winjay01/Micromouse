#include <Arduino.h>
#include "floodfill.h"

int next_cell_dir(int curr_dir, int next_dir) {
  if (curr_dir == NORTH) {
    switch (next_dir) {
      case NORTH: return FORWARD;
      case EAST: return RIGHT;
      case SOUTH: return 5;
      case WEST: return LEFT;
      default: return 5;
    }
  }
  else if (curr_dir == EAST) {
    switch (next_dir) {
      case NORTH: return LEFT;
      case EAST: return FORWARD;
      case SOUTH: return RIGHT;
      case WEST: return 5;
      default: return 5;
    }
  }
  else if (curr_dir == SOUTH) {
    switch (next_dir) {
      case NORTH: return 5;
      case EAST: return LEFT;
      case SOUTH: return FORWARD;
      case WEST: return RIGHT;
      default: return 5;
    }
  }
  else if (curr_dir == WEST) {
    switch (next_dir) {
      case NORTH: return RIGHT;
      case EAST: return 5;
      case SOUTH: return LEFT;
      case WEST: return FORWARD;
      default: return 5;
    }
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

// Find adjacent cell with lowest path cost
int min_dir(int row, int col) {
  int min_c = MAZE_SIZE;
  int min_d = 5;
  int current_cell = row*MAZE_DIM + col;

  // Check cell to right
  if (col < (MAZE_DIM - 1)) {
    if (!check_bit(walls[current_cell], EAST) && path_cost[row][col + 1] < min_c) {
      min_c = path_cost[row][col + 1];
      min_d = EAST;
    }
  }
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
  return min_d;
}

// Continuously move to adjacent cell with lowest path cost until wall is detected or center is reached
int find_path(int current_cell, int i, byte path[MAZE_SIZE], int returning) {
  int row = current_cell / MAZE_DIM;
  int col = current_cell % MAZE_DIM;
  path[i++] = current_cell;

  while ((!returning) ? (!center_reached(current_cell)) : (current_cell != 0)) {
    int dir = min_dir(row, col);
    switch (dir) {
      case NORTH: current_cell = (row-1)*MAZE_DIM + col; break;
      case EAST: current_cell = row*MAZE_DIM + (col + 1); break;
      case SOUTH: current_cell = (row+1)*MAZE_DIM + col; break;
      case WEST: current_cell = row*MAZE_DIM + (col - 1); break;
      //default: Serial.print(F("No dir found, cell: ")); Serial.print(row); Serial.print(F(" ")); Serial.println(col); break;
    }
    if (in_path(current_cell, path)) { // Backtracking
      if (!(returning && (current_cell == 0))) {
        clear_path(path, (i - 1));
        //Sensors.blink(Sensors.LED_L, 500, 1);
        return 1;
      }
    }
    path[i++] = current_cell;
    row = current_cell / MAZE_DIM;
    col = current_cell % MAZE_DIM;
  }
  return 0;
}

void print_path(byte path[MAZE_SIZE], int current_cell) {
  for (int i = 0; i < 2; i++) Serial.println();
  int cell;
  for (int i = 0; i < MAZE_DIM; i++) {
    for (int j = 0; j < MAZE_DIM; j++) {
      cell = i*MAZE_DIM + j;
      if (cell == current_cell) Serial.print(F("X  "));
      else if (in_path(cell, path)) Serial.print(F("*  "));
      else {
        Serial.print(path_cost[i][j]);
        Serial.print(F(" "));
        if (path_cost[i][j] <= 9) Serial.print(F(" ")); // To line up w/ 2 digit nums
      }
    }
    Serial.println();
  }
}