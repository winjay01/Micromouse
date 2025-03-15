#include "Arduino.h"
#include "motors.h"
#include "encoders.h"
#include <math.h>

motors::motors() {
  FORWARD = 1;
  REVERSE = -1;
  FW = 0;
  B = 1;
  L = 2;
  R = 3;
  LPWM1 = 10;
  LPWM2 = 11;
  RPWM1 = 9;
  RPWM2 = 6;
}

void motors::SETUP() {
  pinMode(RPWM1, OUTPUT);
  pinMode(RPWM2, OUTPUT);
  pinMode(LPWM1, OUTPUT);
  pinMode(LPWM2, OUTPUT);
}

void motors::setMotorR(int dir, int pwm_val) {
  // Forward
  if (dir == FORWARD) {
    analogWrite(RPWM1, pwm_val);
    digitalWrite(RPWM2, LOW);
  }
  // Reverse
  else if (dir == REVERSE) {
    analogWrite(RPWM2, pwm_val);
    digitalWrite(RPWM1, LOW);
  }
}

void motors::setMotorL(int dir, int pwm_val) {
  // Forward
  if (dir == FORWARD) {
    analogWrite(LPWM1, pwm_val);
    digitalWrite(LPWM2, LOW);
  }
  // Reverse
  else if (dir == REVERSE) {
    analogWrite(LPWM2, pwm_val);
    digitalWrite(LPWM1, LOW);
  }
}

void motors::stop() {
  digitalWrite(RPWM1, LOW);
  digitalWrite(RPWM2, LOW);
  digitalWrite(LPWM1, LOW);
  digitalWrite(LPWM2, LOW);
}

void motors::turn_right(int speed) {  
  setMotorL(FORWARD, speed);
  setMotorR(REVERSE, speed);
}

void motors::turn_left(int speed) {
  setMotorL(REVERSE, speed);
  setMotorR(FORWARD, speed);
}

void motors::forward(int speed) {
  setMotorL(FORWARD, speed);
  setMotorR(FORWARD, speed);
}

void motors::reverse(int speed) {
  setMotorL(REVERSE, speed);
  setMotorR(REVERSE, speed);
}

void motors::drive(int dir, int speed) {
  if (dir == FW) forward(speed);
  else if (dir == B) reverse(speed);
  else if (dir == L) turn_left(speed);
  else if (dir == R) turn_right(speed);
}

void motors::drive_pos(int enc_steps, int dir, int speed) {
  int pos = Encoders.getPosR();
  digitalWrite(LED_BUILTIN, HIGH);
  drive(dir, speed);
  int steps = fabs(Encoders.getPosR() - pos);
  while(steps < enc_steps) {
    /*
    Serial.print(F("Pos L: "));
    Serial.print(Encoders.getPosL());
    Serial.print(F(" Pos R: "));
    Serial.println(Encoders.getPosR());
    */
    steps = fabs(Encoders.getPosR() - pos);
  }
  stop();
  /*
  Serial.print(F("END*** Pos L: "));
  Serial.print(Encoders.getPosL());
  Serial.print(F(" Pos R: "));
  Serial.println(Encoders.getPosR());
  */
  digitalWrite(LED_BUILTIN, LOW);
}

void motors::drive_for(int time, int dir, int speed) {
  digitalWrite(LED_BUILTIN, HIGH);
  drive(dir, speed);
  delay(time);
  stop();
  digitalWrite(LED_BUILTIN, LOW);
}

motors Motors = motors();