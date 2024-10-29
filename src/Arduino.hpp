#include "floodfill.hpp"

#define ENC1A 2
#define ENC1B 3
#define ENC2A 4
#define ENC2B 5

#define PWM1 6
#define IN1 7
#define PWM2 8
#define IN2 9

int posL = 0; // encoder count for left motor
int posR = 0; // encoder count for right motor

void setup() {
  pinMode(ENC1A, INPUT);
  pinMode(ENC1B, INPUT);
  pinMode(ENC2A, INPUT);
  pinMode(ENC2B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC1A), readEncoder, RISING);
}

void loop() {
  //
}

void setMotor(int dir, int pwm_val, int pwm1, int pwm2) {
  if (dir == 1) {
    analogWrite(pwm1, pwm_val);
    digitalWrite(pwm2, LOW);
  }
  else if (dir == -1) {
    analogWrite(pwm2, pwm_val);
    digitalWrite(pwm1, LOW);
  }
}

void readEncoder() {
  int b = digitalRead(ENC1B);
  if (b > 0) {
    posL++;
  }
  else {
    posL--;
  }
}