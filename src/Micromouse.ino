#include <math.h>
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include "pid_control.h"
#include "traverse.h"

#define STD_D 1000
#define FORWARD 0
#define LEFT 1
#define RIGHT 2

int drove;

void setup() {
  Serial.begin(9600);
  Sensors.SETUP();
  Motors.SETUP();
  Encoders.SETUP();
  Sensors.blink(LED_BUILTIN, STD_D, 1);
  drove = 0;
}

void loop() {
  //PD.control_loop(LEFT);
  //Sensors.read_all();
  if (!drove) {
    run();
    drove = 1;
  }
}

void right_90(int speed) {
  int currPos = Encoders.getPosL();
  int init = currPos;
  Motors.turn_right(speed);
  while ((currPos - init) < 20) {
    //Serial.println(currPos);
    currPos = Encoders.getPosL();
  }
  delay(100);
  Motors.stop();
}