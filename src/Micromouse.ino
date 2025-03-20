#include <math.h>
#include "traverse.h"
#include "encoders.h"

#define STD_D 1000

int test = 1;

void setup() {
  Serial.begin(9600);
  Sensors.SETUP();
  Motors.SETUP();
  Encoders.SETUP();
  Sensors.blink(LED_BUILTIN, STD_D, 1);
}

void loop() {
  //PD.control_loop(PD.FORWARD);
  if (test) {
    //Motors.drive_for(STD_D, Motors.FW, 70);
    //wall_int(6, SOUTH, FORWARD);
    //wall_int(6, EAST, FORWARD);
    //print_walls();
    //floodfill();
    run();
    //move(PD.FORWARD, 1);
    test = 0;
  }
  //Sensors.read_all();
  //delay(Sensors.TIMER);
}