#include "pid_control.h"
#include "motors.h"
#include "encoders.h"
#include <Arduino.h>
#include <math.h>

const int forw = 1; // (Individual motor)
const int rev = -1; // (Individual motor)

pid_control::pid_control() {
  cell_steps = 55;
  turn_diff = 45;
  // PID constants: Distance | Angle | Turn
  float Kp[3] = {4, 3, 4};
  float Ki[3] = {0, 0, 0};
  float Kd[3] = {0.25, 0, 0.25};
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

void pid_control::RESET() {
  Encoders.RESET();
  prevT = -1;

}

int pid_control::error_signal(int mode) {
  if (mode == DISTANCE) return cell_steps - ((Encoders.getPosL() + Encoders.getPosR())/2); 
  if (mode == ANGLE) return Encoders.getPosL() - Encoders.getPosR();
  if (mode == TURN) return turn_diff - fabs(Encoders.getPosL() - Encoders.getPosR());
  return 0;
}

int pid_control::control_signal(int mode, float dt) {
  int e;
  // Compute proportional error (e(t))
  if (mode == DISTANCE) e = error_signal(DISTANCE);
  else if (mode == ANGLE) e = error_signal(ANGLE);
  else if (mode == TURN) e = error_signal(TURN);
  // Compute integral error
  eint[mode] += e*dt;
  // Compute differential error (de(t) / dt)
  float dedt = (e - eprev[mode])/dt;
  eprev[mode] = e;

  // u(t) = ...
  //Serial.print(e);
  //Serial.print(" ");
  //Serial.print(dedt);
  return kp[mode]*e + ki[mode]*eint[mode] + kd[mode]*dedt;
}

void pid_control::control_loop(int dir) {
  long currT = millis();
  if (prevT == -1) prevT = currT;
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
    // Make sure it moves
    if ((error_signal(TURN) != 0) && (pwrt < 60)) pwrt = 60;
    //Serial.println(pwrt);

    if (dir == LEFT) {
      if (ut > 0) { // Under target
        Motors.setMotorL(rev, pwrt);
        Motors.setMotorR(forw, pwrt);
      }
      else { // Over target
        Motors.setMotorL(forw, pwrt);
        Motors.setMotorR(rev, pwrt);
      }
    } 
    else if (dir == RIGHT) {
      if (ut > 0) { // Under target
        Motors.setMotorL(forw, pwrt);
        Motors.setMotorR(rev, pwrt);
      }
      else { // Over target
        Motors.setMotorL(rev, pwrt);
        Motors.setMotorR(forw, pwrt);
      }
    }
  }
  else { // Drive straight
    // Distance value
    float pwr_d = fabs(ud);
    if (pwr_d > 255) pwr_d = 255;
    if ((error_signal(DISTANCE) != 0) && (pwr_d < 35)) pwr_d = 35;
    // Angle values
    float pwr_L = pwr_d - ua;
    float pwr_R = pwr_d + ua;
    if (pwr_L > 255) pwr_L = 255;
    if (pwr_R > 255) pwr_R = 255;
    if (pwr_L < 0) pwr_L = 0;
    if (pwr_R < 0) pwr_R = 0;
    /*
    if (error_signal(ANGLE) != 0) {
      int diff;
      if ((pwr_L > pwr_R) && (pwr_R < 40)) {
        diff = 40 - pwr_R;
        pwr_R = 40;
        pwr_L += diff;
      }
      if ((pwr_L < pwr_R) && (pwr_L < 40)) {
        diff = 40 - pwr_L;
        pwr_L = 40;
        pwr_R += diff;
      }
    }
    */
    if (ud > 0) { // Under target
      Motors.setMotorL(forw, pwr_L);
      Motors.setMotorR(forw, pwr_R);
    }
    else if (ud <= 0) { // Over target
      Motors.setMotorL(rev, pwr_R);
      Motors.setMotorR(rev, pwr_L);
    }
  }
  Serial.print(ud);
  Serial.print(" ");
  Serial.print(Encoders.getPosL());
  Serial.print(" ");
  Serial.println(Encoders.getPosR());
  
}

int pid_control::dir2mode(int dir) {
  if (dir == FORWARD) return DISTANCE;
  else return TURN;
}

pid_control PD = pid_control();