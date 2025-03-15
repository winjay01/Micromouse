#include <math.h>
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include "pid_control.h"
#include "traverse.h"

#define STD_D 1000

int directions[] = {PD.FORWARD, PD.LEFT, PD.FORWARD, PD.RIGHT, PD.FORWARD};
int index;

int test = 1;

void setup() {
  Serial.begin(9600);
  Sensors.SETUP();
  Motors.SETUP();
  Encoders.SETUP();
  index = 0;
  Sensors.blink(LED_BUILTIN, STD_D, 1);
}

void loop() {
  if (test) {
    run();
    //move(PD.FORWARD, 1);
    test = 0;
  }
}