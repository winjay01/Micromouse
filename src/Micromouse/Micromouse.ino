#include <math.h>
#include "queue.h"
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include "pid_control.h"

#define STD_D 1000
// Directions
#define F 0
#define B 1
#define L 2
#define R 3

int drove = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Motors.SETUP();
  Encoders.SETUP();
  PD.SETUP(2, 0, 0.1); // Kp, Ki, Kd
}

void loop() {
  //PD.control_loop();

  if (!drove) {
    drive_for(750, F, 30);
    //drive_pos(30, F, 30);
    delay(STD_D);

    drive_for(750, B, 30);
    //drive_pos(30, B, 30);
    delay(STD_D);
    //drove = 1;
  }
}

void drive(int dir, int speed) {
  switch (dir) {
    case F: Motors.forward(speed); break;
    case B: Motors.reverse(speed); break;
    case L: Motors.turn_left(speed); break;
    case R: Motors.turn_right(speed); break;
  }
}

// Drive a certain distance
void drive_pos(int enc_steps, int dir, int speed) {
  int pos = Encoders.getPosR();
  digitalWrite(LED_BUILTIN, HIGH);
  drive(dir, speed);
  int steps = fabs(Encoders.getPosR() - pos);
  while(steps < enc_steps) {
    Serial.println(steps);
    steps = fabs(Encoders.getPosR() - pos);
  }
  Motors.stop();
  digitalWrite(LED_BUILTIN, LOW);
}

// Drive for a certain amount of time
void drive_for(int time, int dir, int speed) {
  digitalWrite(LED_BUILTIN, HIGH);
  drive(dir, speed);
  delay(time);
  Motors.stop();
  digitalWrite(LED_BUILTIN, LOW);
}