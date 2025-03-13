#include "pid_control.h"
#include "motors.h"
#include "encoders.h"
#include <Arduino.h>
#include <math.h>

long prevT = 0;
float eprev[3] = {0, 0, 0};
float eint[3] = {0, 0, 0}; // Integral error
// Encoder counts needed for 1 cell
const int cell_steps = 80;
// Directions
const int F = 1;
const int R = -1;
// Modes
const int DISTANCE = 0;
const int ANGLE = 1;
const int TURN = 2;

pid_control::pid_control() {
}

void pid_control::SETUP(float KP[3], float KI[3], float KD[3]) {
  for (int i = 0; i < 3; i++) {
    kp[i] = KP[i];
    ki[i] = KI[i];
    kd[i] = KD[i];
  }
}

int pid_control::control_signal(int mode, float dt) {
  // Compute proportional error (e(t))
  int e;
  if (mode == DISTANCE) e = cell_steps - ((Encoders.getPosL() + Encoders.getPosR())/2);
  else if (mode == ANGLE) e = Encoders.getPosL() - Encoders.getPosR();
  // Compute integral error
  eint[mode] += e*dt;
  // Compute differential error (de(t) / dt)
  float dedt = (e - eprev[mode])/dt;

  eprev[mode] = e;
  // u(t) = ...
  //Serial.println("dedt: " + (String)dedt);
  return kp[mode]*e + ki[mode]*eint[mode] + kd[mode]*dedt;
}

void pid_control::control_loop() {
  long currT = millis();
  float dt = ((float)(currT - prevT))/1000;
  prevT = currT;
  // Compute control signal (u(t))
  float ud = control_signal(DISTANCE, dt);
  float ua = control_signal(ANGLE, dt);

  // Cap control signal
  float pwr_d = fabs(ud);
  if (pwr_d > 255) pwr_d = 255;
  float pwr_L = pwr_d - ua;
  float pwr_R = pwr_d + ua;
  if (pwr_L > 255) pwr_L = 255;
  if (pwr_R > 255) pwr_R = 255;
  if (pwr_L < 0) pwr_L = 0;
  if (pwr_R < 0) pwr_R = 0;

  // Drive motors using control signal
  if (ud > 0) {
    Motors.setMotorL(F, pwr_L);
    Motors.setMotorR(F, pwr_R);
  }
  else {
    Motors.setMotorL(R, pwr_R);
    Motors.setMotorR(R, pwr_L);
  }
  Serial.println("ud: " + (String)ud + " ua: " + (String)ua + " PosL: " + (String)Encoders.getPosL() + " PosR: " + (String)Encoders.getPosR());
}

pid_control PD = pid_control();