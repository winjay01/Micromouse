#include <Arduino.h>
#include "movement.h"
#include "pathfinding.h"

byte curr_dir; // which way are we facing with respect to center
int j, backtracking;

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
      case WEST: turn(PD.LEFT, 2); break;
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

int traverse1(int current_cell, int* i, byte path[MAZE_SIZE]) {
  int next_path_length, dir, actual_length;
  next_path_length = get_num_cells(path, *i);

  while (next_path_length > 0) {
    dir = get_next_dir(current_cell, path[*i + 1]);
    reorient(dir); // Face this direction
    curr_dir = dir;
    actual_length = move0(next_path_length); // Move to next cell
    *i = *i + actual_length;
    current_cell = path[*i];
    next_path_length = get_num_cells(path, *i);
  }
  return current_cell;
}

int backtrack(int current_cell, byte path[MAZE_SIZE], int i) {
  int dir = get_next_dir(current_cell, path[i]);
  reorient(dir);
  curr_dir = dir;
  move0(1);
  return path[i];
}

int check_ahead(int wall_bitmap, int next_dir) {
  int sensor_dir = next_cell_dir(curr_dir, next_dir);
  switch (sensor_dir) {
    case FORWARD: return (check_bit(wall_bitmap, 0) || check_bit(wall_bitmap, 3));
    case LEFT: return (check_bit(wall_bitmap, LEFT));
    case RIGHT: return (check_bit(wall_bitmap, RIGHT));
    default: return 0;
  }
}

// 1 cell at a time
int traverse2(int current_cell, int* i, byte path[MAZE_SIZE], int returning) {
  int clear_ahead = 1;
  int dir = get_next_dir(current_cell, path[*i + 1]);
  while (clear_ahead && ((returning) ? current_cell != 0 : !center_reached(current_cell))) {
    //Serial.print(F("Current cell: ")); Serial.print(current_cell / MAZE_DIM); Serial.print(F(" ")); Serial.print(current_cell % MAZE_DIM);
    //Serial.print(F(" , Facing: ")); Serial.print(dir);
    reorient(dir);
    curr_dir = dir;
    //Serial.print(F(" , Moving to cell ")); Serial.print(path[*i + 1] / MAZE_DIM); Serial.print(F(" ")); Serial.println(path[*i + 1] % MAZE_DIM);
    move0(1);
    *i = *i + 1;
    current_cell = path[*i];
    dir = get_next_dir(current_cell, path[*i + 1]);
    clear_ahead = !check_ahead(check_walls(current_cell, curr_dir), dir);
    if (!clear_ahead) clear_path(path, (*i + 1));
  }
  return current_cell;
}

void run() {
  byte path[MAZE_SIZE]; // save path traversed
  clear_path(path, 0);
  init_layout();
  curr_dir = EAST; // start facing left w/ respect center, no matter which corner robot is dropped in
  //int current_cell = ((MAZE_DIM/2)*MAZE_DIM + (MAZE_DIM/2 - 1)); // start at top left
  int current_cell = 0;
  int i = 0; // tracks path length
  check_walls(current_cell, curr_dir);

  // First search
  while (!center_reached(current_cell)) {
    floodfill(0);
    backtracking = find_path(current_cell, i, path, 0);
    while (backtracking) {
      i = i - 1;
      current_cell = backtrack(current_cell, path, i);
      backtracking = find_path(current_cell, i, path, 0);
    }
    //Serial.print(F("i = ")); Serial.println(i); print_path(path, current_cell);
    current_cell = traverse2(current_cell, &i, path, 0);
  }
  // Now in center, find potential shorter path
  byte new_path[MAZE_SIZE];
  clear_path(new_path, 0);
  j = 0;
  while (current_cell != 0) {
    floodfill(1);
    backtracking = find_path(current_cell, j, new_path, 1);
    while (backtracking) {
      j = j - 1;
      current_cell = backtrack(current_cell, new_path, j);
      backtracking = find_path(current_cell, j, new_path, 1);
    }
    current_cell = traverse2(current_cell, &j, new_path, 1);
  }
}