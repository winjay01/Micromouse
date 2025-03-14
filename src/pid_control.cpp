#include "pid_control.h"
#include "motors.h"
#include "encoders.h"
#include <Arduino.h>
#include <math.h>

pid_control::pid_control() {
  cell_steps = 80;
  turn_diff = 40;
  // PID constants: Distance | Angle | Turn
  float Kp[3] = {2, 3, 2};
  float Ki[3] = {0, 0, 0};
  float Kd[3] = {0.2, 0, 0};
  for (int i = 0; i < 3; i++) {
    kp[i] = Kp[i];
    ki[i] = Ki[i];
    kd[i] = Kd[i];
    eprev[i] = 0;
    eint[i] = 0;
  }
  FORWARD = 0;
  LEFT = 1;
  RIGHT = 2;
  DISTANCE = 0;
  ANGLE = 1;
  TURN = 2;
  prevT = 0;
}

int pid_control::control_signal(int mode, float dt) {
  int e;
  // Compute proportional error (e(t))
  if (mode == DISTANCE) e = cell_steps - ((Encoders.getPosL() + Encoders.getPosR())/2);
  else if (mode == ANGLE) e = Encoders.getPosL() - Encoders.getPosR();
  else if (mode == TURN) e = turn_diff - fabs(Encoders.getPosL() - Encoders.getPosR());
  // Compute integral error
  eint[mode] += e*dt;
  // Compute differential error (de(t) / dt)
  float dedt = (e - eprev[mode])/dt;
  eprev[mode] = e;

  // u(t) = ...
  return kp[mode]*e + ki[mode]*eint[mode] + kd[mode]*dedt;
}

void pid_control::control_loop(int dir) {
  const int F = 1; // (Individual motor)
  const int R = -1; // (Individual motor)
  long currT = millis();
  float dt = ((float)(currT - prevT))/1000;
  prevT = currT;
  // Compute control signal (u(t))
  float ud = control_signal(DISTANCE, dt);
  float ua = control_signal(ANGLE, dt);
  float ut = control_signal(TURN, dt);

  // Drive motors using control signal
  if (dir != FORWARD) { // Turning
    float pwrt = fabs(ut);
    if (pwrt > 255) pwrt = 255;
    if (dir == LEFT) {
      if (ut > 0) { // Under target
        Motors.setMotorL(R, pwrt);
        Motors.setMotorR(F, pwrt);
      }
      else { // Over target
        Motors.setMotorL(F, pwrt);
        Motors.setMotorR(R, pwrt);
      }
    } 
    else if (dir == RIGHT) {
      if (ut > 0) { // Under target
        Motors.setMotorL(F, pwrt);
        Motors.setMotorR(R, pwrt);
      }
      else { // Over target
        Motors.setMotorL(R, pwrt);
        Motors.setMotorR(F, pwrt);
      }
    }
  }
  else { // Drive straight
    // Distance value
    float pwr_d = fabs(ud);
    if (pwr_d > 255) pwr_d = 255;
    // Angle values
    float pwr_L = pwr_d - ua;
    float pwr_R = pwr_d + ua;
    if (pwr_L > 255) pwr_L = 255;
    if (pwr_R > 255) pwr_R = 255;
    if (pwr_L < 0) pwr_L = 0;
    if (pwr_R < 0) pwr_R = 0;

    if (ud > 0) { // Under target
      Motors.setMotorL(F, pwr_L);
      Motors.setMotorR(F, pwr_R);
    }
    else if (ud <= 0) { // Over target
      Motors.setMotorL(R, pwr_R);
      Motors.setMotorR(R, pwr_L);
    }
  }
  //Serial.println("ud: " + (String)ud + " ua: " + (String)ua + " PosL: " + (String)Encoders.getPosL() + " PosR: " + (String)Encoders.getPosR());
}

pid_control PD = pid_control();