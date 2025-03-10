#include "pid_control.h"
#include "motors.h"
#include "encoders.h"
#include <Arduino.h>
#include <math.h>

long prev_time = 0;
int error_prev = 10000;
const int cell_steps = 50;
const int F = 1;

pid_control::pid_control() {
}

void pid_control::SETUP(int KP, int KI, int KD) {
  kp = KP;
  ki = KI;
  kd = KD;
}

int pid_control::angle_control(int kp, int kd, int curr_time, int prev_time) {
  float delta_time = ((float)(curr_time - prev_time))/1.0e6;

  // Compute proportional error (e(t))
  int error = 0 - (Encoders.getPosL() - Encoders.getPosR());
  if (error_prev == 10000) error_prev = error;

  // Compute differential error (de(t) / dt)
  float dedt = (error - error_prev)/delta_time;
  error_prev = error;

  // u(t) = ...
  return kp*error + kd*dedt;
}

int pid_control::distance_control(int kp, int kd, int curr_time, int prev_time) {
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
  float ud = distance_control(kp, kd, curr_time, prev_time);
  float ua = angle_control(kp, kd, curr_time, prev_time);
  prev_time = curr_time;

  // Cap control signals
  float pwr_d = fabs(ud);
  float pwr_a = fabs(ua);
  if (pwr_d > 255) pwr_d = 255;
  if (pwr_a > 255) pwr_a = 255;

  float comb = pwr_a + pwr_d;
  if (comb > 255) comb = 255;
  // cant be too low or else car wont move
  if (pwr_d < 30) pwr_d = 30;
  if (comb < 30) comb = 30;

  // Drive motors using control signal
  if (ud > 0) {
    Motors.forward(pwr_d);
    if (ua < 0) Motors.setMotorR(F, comb);
    else if (ua > 0) Motors.setMotorL(F, comb);
  }
  else if (ud < 0) {
    Motors.reverse(pwr_d);
    //if (ua < 0) Motors.setMotorR(F, comb);
    //else if (ua > 0) Motors.setMotorL(F, comb);
  }

  //Serial.println("Steps: " + (String)cell_steps + " PosL: " + (String)Encoders.getPosL() + " PWR: " + (String)pwr + " KP KD: " + (String)kp + " " + (String)kd);
}

pid_control PD = pid_control();