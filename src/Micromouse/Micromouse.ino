#include <math.h>
#include "queue.h"
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include "pid_control.h"

#define STD_D 1000
// PID constants: Distance | Angle | Turn
const float Kp[3] = {2, 3, 0};
const float Ki[3] = {0, 0, 0};
const float Kd[3] = {0.2, 0, 0};

void setup() {
  Sensors.SETUP();
  Motors.SETUP();
  Encoders.SETUP();
  PD.SETUP(Kp, Ki, Kd); `
  Sensors.blink(LED_BUILTIN, STD_D, 1);
}

void loop() {
  PD.control_loop();
  Sensors.read_all();
}

void right_90(int speed) {
  int currPos = Encoders.getPosL();
  int init = currPos;
  Motors.turn_right(speed);
  while ((currPos - init) < 20) {
    Serial.println(currPos);
    currPos = Encoders.getPosL();
  }
  delay(100);
  Motors.stop();
}