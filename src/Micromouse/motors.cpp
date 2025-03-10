#include "Arduino.h"
#include "motors.h"

const int RPWM1 = 11;
const int RPWM2 = 10;
const int LPWM1 = 6;
const int LPWM2 = 9;

const int FORWARD = 1;
const int REVERSE = -1;

motors::motors() {
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
  setMotorR(REVERSE, speed);
  setMotorL(FORWARD, speed);
}

void motors::turn_left(int speed) {
  setMotorR(FORWARD, speed);
  setMotorL(REVERSE, speed);
}

void motors::forward(int speed) {
  setMotorR(FORWARD, speed);
  setMotorL(FORWARD, speed);
}

void motors::reverse(int speed) {
  setMotorR(REVERSE, speed);
  setMotorL(REVERSE, speed);
}

motors Motors = motors();