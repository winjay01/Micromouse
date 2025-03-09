#include "pid_control.h"
#include "motors.h"
#include "encoders.h"
#include <Arduino.h>
#include <math.h>

long prev_time = 0;
int error_prev = 10000;
const int cell_steps = 30;

pid_control::pid_control() {
}

void pid_control::SETUP(int KP, int KI, int KD) {
  kp = KP;
  ki = KI;
  kd = KD;
}

int pid_control::pd_control(int kp, int kd, int curr_time, int prev_time) {
  float delta_time = ((float)(curr_time - prev_time))/1.0e6;

  // Compute proportional error (e(t))
  int error = cell_steps - Encoders.getPosL();
  if (error_prev == 10000) error_prev = error;

  // Compute differential error (de(t) / dt)
  float dedt = (error - error_prev)/delta_time;
  error_prev = error;

  // u(t) = ...
  return kp*error + kd*dedt;
}

void pid_control::control_loop() {
  long curr_time = micros();
  // Compute control signal (u(t))
  u = pd_control(kp, kd, curr_time, prev_time);
  prev_time = curr_time;

  // Cap control signal
  float pwr = fabs(u);
  if (pwr > 255) pwr = 255;
  // cant be too low or else car wont move
  if (pwr < 30) pwr = 30;

  // Determine if we overshot
  int dir = 1;
  if (u < 0) dir = -1;

  // Drive motor using control signal
  if (dir == 1) {
    Motors.forward(pwr);
  }
  else {
    Motors.reverse(pwr);
  }

  //Serial.println("Steps: " + (String)cell_steps + " PosL: " + (String)Encoders.getPosL() + " PWR: " + (String)pwr + " KP KD: " + (String)kp + " " + (String)kd);
}

pid_control PD = pid_control();