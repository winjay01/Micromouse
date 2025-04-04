#include <Arduino.h>
#include "sensors.h"

#define MAZE_DIM 12
#define MAZE_SIZE MAZE_DIM*MAZE_DIM
// Maze representation: 16x16 unit grid
byte walls[MAZE_SIZE]; // Array of bitmaps

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

const byte FORWARD = 0;
const byte LEFT = 1;
const byte RIGHT = 2;

void init_layout() {
  for (int i = 0; i < MAZE_SIZE; i++) {
    walls[i] = 0;
  }
}

int check_bit(int num, int n) {
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

int check_walls(int current_cell, int curr_dir) {
  int wall_bitmap = Sensors.read_all();
  if (check_bit(wall_bitmap, 0) || check_bit(wall_bitmap, 3)) {
    digitalWrite(LED_BUILTIN, HIGH);
    wall_int(current_cell, curr_dir, FORWARD);
  }
  if (check_bit(wall_bitmap, LEFT)) {
    wall_int(current_cell, curr_dir, LEFT);
    digitalWrite(Sensors.LED_L, HIGH);
  }
  if (check_bit(wall_bitmap, RIGHT)) {
    digitalWrite(Sensors.LED_R, HIGH);
    wall_int(current_cell, curr_dir, RIGHT);
  }
  //delay(250);
  //Sensors.leds_off();
  //delay(250);
  return wall_bitmap;
}