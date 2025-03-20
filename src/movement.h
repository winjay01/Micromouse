#include <Arduino.h>
#include "pid_control.h"
#include "motors.h"
#include "encoders.h"
#include "sensors.h"

#define DIR_SIZE 5

int reset = 0, reached = 0, stopping = 0;

void steady_state(int dir) {
  int ti = millis();
  int t = ti;
  while((t - ti) < 750) {
    PD.control_loop(dir);
    digitalWrite(LED_BUILTIN, (PD.error_signal(PD.dir2mode(dir)) == 0.0));
    t = millis();
  }
}

void halt(int dir) {
  steady_state(dir);
  reached = 1;
  Motors.stop();
  reset = 0;
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(Sensors.LED_L, LOW);
}

void check_reset() {
  if (!reset) {
    PD.RESET();
    reset = 1;
  }
}

int destination_check(int dist_mode) {
  if (dist_mode == PD.DISTANCE0) return ((fabsf(PD.error_signal(PD.DISTANCE)) == 0.0) && (fabsf(PD.error_signal(PD.ANGLE)) == 0.0));
  if (dist_mode == PD.DISTANCE1) return ((fabsf(PD.error_signal(PD.DISTANCE)) < 10.0) && (fabsf(PD.error_signal(PD.ANGLE)) == 0.0));
  return 0;
}

int move(int dir, int cell_count) {
  PD.set_target(cell_count);
  while (!reached) {
    check_reset();
    PD.control_loop(dir);
    if (destination_check(PD.DISTANCE)) {
      halt(dir);
      cell_count = PD.get_curr_cells(); // In case we stopped early due to wall in path
    }
  }
  reached = 0;
  Sensors.blink(Sensors.LED_R, 500, cell_count);
  return cell_count;
}

void turn(int dir, int count) {
  for (int c = 0; c < count; c++) {
    while (!reached) {
      check_reset();
      PD.control_loop(dir);
      if (fabsf(PD.error_signal(PD.TURN)) == 0.0) halt(dir);
    }
    reached = 0;
  }
}