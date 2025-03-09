#include "traverse.hpp"

// Encoder pins
#define LENCA 2
#define LENCB 3
#define RENCA 4
#define RENCB 5

int cell_count = 10; // # of encoder counts needed to move forward 1 cell
int posL = 0; // encoder count for left motor
int posR = 0; // encoder count for right motor

long prev_time = 0;
int error_prev = 10000;

void setup() {
  pinMode(LENCA, INPUT);
  pinMode(LENCB, INPUT);
  pinMode(RENCA, INPUT);
  pinMode(RENCB, INPUT);
  attachInterrupt(digitalPinToInterrupt(LENCA), readEncoderL, RISING);
  attachInterrupt(digitalPinToInterrupt(RENCA), readEncoderR, RISING);
}

void loop() {

  // PD constants
  float kp = 1;
  float kd = 0;

  long curr_time = micros();
  float u = pd_control(kp, kd, curr_time, prev_time);
  prev_time = curr_time;

  float pwr = fabs(u);
  if (pwr > 255) {
    pwr = 255;
  }

  int dir = 1;
  if (u < 0) {
    dir = -1;
  }
}

int pd_control(int kp, int kd, int curr_time, int prev_time) {
  float delta_time = ((float)(curr_time - prev_time))/1.0e6;


  int error = posL - cell_count;
  if (error_prev == 10000) error_prev = error;
  float dedt = (error - error_prev)/delta_time;
  error_prev = error;
}

void readEncoderL() {
  int b = digitalRead(LENCB);
  if (b > 0) {
    posL++;
  }
  else {
    posL--;
  }
}

void readEncoderR() {
  int b = digitalRead(RENCB);
  if (b > 0) {
    posR++;
  }
  else {
    posR--;
  }
}