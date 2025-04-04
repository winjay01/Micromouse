#include <math.h>
#include "traverse.h"
#include "encoders.h"

#define STD_D 1000

int test = 1;

void setup() {
  //Serial.begin(9600);
  Sensors.SETUP();
  Motors.SETUP();
  Encoders.SETUP();
  Sensors.blink(LED_BUILTIN, STD_D, 1);
}

void loop() {
  if (test) {
    run();
    test = 0;
  }
  Sensors.blink(LED_BUILTIN, STD_D, 1);
}