#include "pid_control.h"
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include <Arduino.h>
#include <math.h>

#define MIN_SPEED 85
#define ENC_TARGET 55
int rL, rR, rF;

pid_control::pid_control() {
  cell_steps = ENC_TARGET;
  target = cell_steps;
  turn_diff = 45;
  // Constants: Distance0 | Angle0 | Turn | Angle1 | Angle2 | Distance1
  float Kp[6] = {1.5, 6.5, 15.0, 0.0, 0.0, 0.6};
  float Ki[6] = {0.0, 0.0, 0.00, 0.0, 0.0, 0.0};
  float Kd[6] = {2.0, 0.0, 0.60, 0.0, 0.0, 0.0};
  for (int i = 0; i < 6; i++) {
    kp[i] = Kp[i];
    ki[i] = Ki[i];
    kd[i] = Kd[i];
    eprev[i] = 0.0;
    eint[i] = 0.0;
  }
  // Directions
  FORWARD = 0;
  LEFT = 1;
  RIGHT = 2;
  // Modes
  DISTANCE0 = 0;
  ANGLE0 = 1;
  TURN = 2;
  ANGLE1 = 3;
  ANGLE2 = 4;
  DISTANCE1 = 5;

  DISTANCE = DISTANCE0;
  ANGLE = ANGLE0;
  prevT = 0;
  pathfinding = 1;
}

void pid_control::set_target(int num_steps) {
  target = num_steps*cell_steps;
}

void pid_control::set_target_precise(int steps) {
  target = steps;
}

void pid_control::RESET() {
  Encoders.RESET();
  prevT = -1;
  for (int i = 0; i < 6; i++) {
    eprev[i] = 0.0;
    eint[i] = 0.0;
  }
}

int pid_control::get_curr_steps() {
  return ((Encoders.getPosL() + Encoders.getPosR())/2);
}

int pid_control::get_curr_cells() {
  for (int i = 0; i <= 15; i++) {
    if (abs(get_curr_steps() - i*cell_steps) < cell_steps/2) return i;
  }
  return 0;
}

void pid_control::stop_in_place() {
  RESET();
  set_target_precise(2);
  int ti = millis();
  int t = ti;
  while((t - ti) < 1000) {
    control_loop(FORWARD);
    t = millis();
  }
  RESET();
}

float pid_control::error_signal(int mode) {
  if (mode == DISTANCE0) return (float)(target - ((Encoders.getPosL() + Encoders.getPosR())/2.0)); 
  if (mode == DISTANCE1) return (float)(Sensors.f_threshold - rF);
  if (mode == ANGLE0) return (float)(Encoders.getPosL() - Encoders.getPosR());
  if (mode == ANGLE1) {
    if (rL > Sensors.threshold) return (float)(Sensors.threshold - rL);
    if (rR > Sensors.threshold) return (float)(Sensors.threshold - rR);
  }
  if (mode == ANGLE2) return 0;
  if (mode == TURN) return (float)(turn_diff - abs(Encoders.getPosL() - Encoders.getPosR()));
  return 0;
}

float pid_control::control_signal(int mode, float dt) {
  // Compute proportional error (e(t))
  float e = error_signal(mode);
  if ((pathfinding == 1) && (mode == DISTANCE0) && (round(e) > cell_steps)) return 60;
  // Compute integral error
  eint[mode] += e*dt;
  // Compute differential error (de(t) / dt)
  float dedt = (e - eprev[mode])/dt;
  eprev[mode] = e;

  // u(t) = ...
  return (kp[mode]*e + ki[mode]*eint[mode] + kd[mode]*dedt);
}

void pid_control::control_loop(int dir) {
  long currT = millis();
  if (prevT == -1) prevT = currT;
  float dt = ((float)(currT - prevT))/1000.0;
  prevT = currT;
  DISTANCE = distance_mode();
  //ANGLE = angle_mode();
  // Compute control signal (u(t))
  float ud = control_signal(DISTANCE, dt);
  float ua = control_signal(ANGLE, dt);
  float ut = control_signal(TURN, dt);

  // Drive motors using control signal
  if (dir == FORWARD) { // Drive straight
    // Distance value
    int pwr_d = (int)fabsf(ud);
    if (pwr_d > 255) pwr_d = 255;
    if ((DISTANCE == DISTANCE0) && (pwr_d < MIN_SPEED) && (fabsf(eprev[DISTANCE]) != 0.0)) pwr_d = MIN_SPEED;
    // Angle values
    int pwr_L = (pwr_d - round(ua));
    int pwr_R = (pwr_d + round(ua));
    if (pwr_L > 255) pwr_L = 255;
    if (pwr_R > 255) pwr_R = 255;
    if (pwr_L < 0) pwr_L = 0;
    if (pwr_R < 0) pwr_R = 0;
    // Make sure it moves
    //digitalWrite(Sensors.LED_R, (eprev[ANGLE] == 0.0));
    //digitalWrite(LED_BUILTIN, (eprev[DISTANCE] == 0.0));
    /*
    if (fabsf(eprev[ANGLE]) == 0.0) { // In line
      if ((DISTANCE == DISTANCE0) && (pwr_L < MIN_SPEED) && (fabsf(eprev[DISTANCE]) != 0.0)) {
        pwr_L = MIN_SPEED;
        pwr_R = MIN_SPEED;
      }
    }
    */
    //else { // Out of line
      if ((eprev[ANGLE] < 0.0) && (pwr_L < MIN_SPEED)) pwr_L = MIN_SPEED;
      if ((eprev[ANGLE] > 0.0) && (pwr_R < MIN_SPEED)) pwr_R = MIN_SPEED; 
    //}
  
    if (ud >= 0.0) { // Under target
      Motors.setMotorL(Motors.FORWARD, pwr_L);
      Motors.setMotorR(Motors.FORWARD, pwr_R);
    }
    else if (ud < 0.0) { // Over target
      Motors.setMotorL(Motors.REVERSE, pwr_R);
      Motors.setMotorR(Motors.REVERSE, pwr_L);
    }
  }
  else if (dir != FORWARD) { // Turning
    int pwrt = (int)fabsf(ut);
    if (pwrt > 255) pwrt = 255;

    if (dir == LEFT) {
      if (ut > 0) { // Under target
        Motors.setMotorL(Motors.REVERSE, pwrt);
        Motors.setMotorR(Motors.FORWARD, pwrt);
      }
      else { // Over target
        Motors.setMotorL(Motors.FORWARD, pwrt);
        Motors.setMotorR(Motors.REVERSE, pwrt);
      }
    } 
    else if (dir == RIGHT) {
      if (ut > 0) { // Under target
        Motors.setMotorL(Motors.FORWARD, pwrt);
        Motors.setMotorR(Motors.REVERSE, pwrt);
      }
      else { // Over target
        Motors.setMotorL(Motors.REVERSE, pwrt);
        Motors.setMotorR(Motors.FORWARD, pwrt);
      }
    }
  }
}

int pid_control::dir2mode(int dir) {
  if (dir == FORWARD) return DISTANCE;
  else return TURN;
}

int pid_control::distance_mode() {
  rF = ((Sensors.read_sensor(Sensors.IR_FL) + Sensors.read_sensor(Sensors.IR_FR))/2);
  //rF = Sensors.read_sensor(Sensors.IR_FR);
  digitalWrite(Sensors.LED_L, (rF >= Sensors.f_limit));
  // No wall in front
  if (rF < Sensors.f_limit) return DISTANCE0;
  // Wall in front
  else return DISTANCE1;
}

int pid_control::angle_mode() {
  rL = Sensors.read_sensor(Sensors.IR_L);
  rR = Sensors.read_sensor(Sensors.IR_R);
  // Neither detect a wall
  if ((rL < Sensors.threshold) && (rR < Sensors.threshold)) return ANGLE0;
  // XOR left sensor detects a wall, right sensor detects a wall
  else if ((rL > Sensors.threshold) ^ (rR > Sensors.threshold)) return ANGLE1;
  // Both detect a wall
  else return ANGLE2;
}

pid_control PD = pid_control();