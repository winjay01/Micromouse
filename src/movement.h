#include <Arduino.h>
#include "pid_control.h"

#define DIR_SIZE 5

int reset = 0, reached = 0;

void steady_state(int dir) {
  int ti = millis();
  int t = ti;
  while((t - ti) < 500) {
    PD.control_loop(dir);
    if (PD.error_signal(PD.dir2mode(dir)) == 0) digitalWrite(LED_BUILTIN, HIGH);
    else digitalWrite(LED_BUILTIN, LOW);
    t = millis();
  }
}

void halt(int dir) {
  steady_state(dir);
  reached = 1;
  Motors.stop();
  reset = 0;
  digitalWrite(LED_BUILTIN, LOW);
}

void check_reset() {
  if (!reset) {
    PD.RESET();
    reset = 1;
  }
}

void move(int dir, int cell_count) {
  for (int c = 0; c < cell_count; c++) {
    while (!reached) {
      /*
      Serial.print("Moving in direction: ");
      if (dir == PD.FORWARD) Serial.println("FORWARD");
      if (dir == PD.LEFT) Serial.println("LEFT");
      if (dir == PD.RIGHT) Serial.println("RIGHT");
      */
      check_reset();
      PD.control_loop(dir);
      if (PD.error_signal(PD.dir2mode(dir)) == 0) halt(dir);
    }
    reached = 0;
  }
}