#include <Arduino.h>
#include "pid_control.h"
#include "motors.h"
#include "encoders.h"
#include "sensors.h"

int reset = 0, reached = 0;

int angle_check() {
  if (PD.ANGLE == PD.ANGLE0) return (fabsf(PD.error_signal(PD.ANGLE)) < 5.0);
  //if (PD.ANGLE == PD.ANGLE1) return (fabsf(PD.error_signal(PD.ANGLE)) < 15.0);
  //if (PD.ANGLE == PD.ANGLE2) return (fabsf(PD.error_signal(PD.ANGLE)) < 15.0);
  return 1;
}

int destination_check() {
  if (PD.DISTANCE == PD.DISTANCE0) return (angle_check() && (fabsf(PD.error_signal(PD.DISTANCE)) == 0.0));
  if (PD.DISTANCE == PD.DISTANCE1) return (angle_check() && (fabsf(PD.error_signal(PD.DISTANCE)) < 10.0));
  return 0;
}

int target_reached(int dir, int dt) {
  if (dir == PD.FORWARD) return ((PD.recentering && (dt > 1000)) || destination_check());
  if (dir != PD.FORWARD) return (fabsf(PD.error_signal(PD.TURN)) == 0.0);
}

void steady_state(int dir) {
  int ti = millis();
  int t = ti;
  // If t >= settling time, leave loop when error = 0, otherwise stay
  while (((t - ti) < 500) ? true : !target_reached(dir, (t - ti))) {
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
  Sensors.leds_off();
}

void check_reset() {
  if (!reset) {
    PD.RESET();
    reset = 1;
  }
}

int move0(int cell_count) {
  PD.set_target(cell_count);
  while (!reached) {
    check_reset();
    PD.control_loop(PD.FORWARD);
    // Update maze layout if we detect walls
    if (destination_check()) {
      halt(PD.FORWARD);
      cell_count = PD.get_curr_cells(); // In case we stopped early due to wall in path
    }
  }
  reached = 0;
  return cell_count;
}

void recenter() {
  PD.recentering = 1;
  PD.set_target_precise(-10);
  int ti = millis();
  int t = ti;
  while (!reached || ((t - ti) > 2000)) {
    check_reset();
    PD.control_loop(PD.FORWARD);
    if (destination_check()) halt(PD.FORWARD);
    t = millis();
  }
  reached = 0;
  PD.recentering = 0;
  PD.set_target(0);
}

void turn(int dir, int count) {
  for (int c = 0; c < count; c++) {
    while (!reached) {
      check_reset();
      PD.control_loop(dir);
      if (fabsf(PD.error_signal(PD.TURN)) == 0.0) halt(dir);
    }
    reached = 0;
    recenter();
  }
}