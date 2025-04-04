#include "pid_control.h"
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include <Arduino.h>
#include <math.h>

#define ENC_TARGET 58 // 18 cm
int MIN_SPEED = 80;
int MIN_SPEED1 = 60;
int MIN_SPEED_TURN = 95;
float CRUISE_SPEED = 45.0;
int ANGLEL = 5;
int ANGLER = 6;
int rL, rR, rF, enc_offset;
int stopped = 0;
int prev_a = 10;
int savet = 0;
long init_time = 0;
long timer = 0;

pid_control::pid_control() {
  cell_steps = ENC_TARGET;
  target = cell_steps;
  turn_diff = 44;
  // Constants: Distance0 | Angle0 | Turn | Angle1 | Angle2 | Distance1
  float Kp[6] = {1.5, 6.0, 5.5, 0.15, 0.15, 0.2};
  float Ki[6] = {0.0, 0.0, 0.0, 0.00, 0.00, 0.0};
  float Kd[6] = {2.0, 0.1, 0.2, 0.05, 0.05, 0.0};
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
  recentering = 0;
  enc_offset = 0;
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
  enc_offset = 0;
  stopped = 0;
  for (int i = 0; i < 6; i++) {
    eprev[i] = 0.0;
    eint[i] = 0.0;
  }
  prev_a = 10;
  savet = 0;
}

int pid_control::get_curr_steps() {
  return ((Encoders.getPosL() + Encoders.getPosR())/2);
}

int pid_control::get_curr_cells() {
  for (int i = 0; i <= 15; i++) {
    if (abs(get_curr_steps() - i*cell_steps) < 20) return i;
  }
  return 0;
}

float pid_control::error_signal(int mode) {
  int enc_diff = (Encoders.getPosL() - Encoders.getPosR());
  float w1 = 0.3;
  float w2 = 0.5;
  if (mode == DISTANCE0) return (float)(target - ((Encoders.getPosL() + Encoders.getPosR())/2.0)); 
  if (mode == DISTANCE1) return (float)(Sensors.f_threshold - rF);
  if (mode == ANGLE0) return (float)(enc_diff - enc_offset);
  if (mode == ANGLE1) {
    if (rL > Sensors.limit) return (w1*(float)(Sensors.threshold - rL) + (1.0-w1)*(float)(enc_diff));
    if (rR > Sensors.limit) return (w1*(float)(rR - Sensors.threshold) + (1.0-w1)*(float)(enc_diff));
  }
  if (mode == ANGLE2) return (w2*(float)(rR - rL) + (1.0-w2)*(float)(enc_diff));
  if (mode == TURN) return (float)(turn_diff - abs(enc_diff));
  return 0;
}

float pid_control::control_signal(int mode, float dt) {
  // Compute proportional error (e(t))
  float e = error_signal(mode);
  // Compute integral error
  eint[mode] += e*dt;
  // Compute differential error (de(t) / dt)
  float dedt = (e - eprev[mode])/dt;
  eprev[mode] = e;

  //if ((pathfinding == 1) && (mode == DISTANCE0) && (round(e) > cell_steps)) return CRUISE_SPEED;
  // u(t) = ...
  return (kp[mode]*e + ki[mode]*eint[mode] + kd[mode]*dedt);
}

void pid_control::control_loop(int dir) {
  long currT = millis();
  if (prevT == -1) prevT = currT;
  float dt = ((float)(currT - prevT))/1000.0;
  prevT = currT;
  DISTANCE = distance_mode();
  if ((dir == FORWARD) && (DISTANCE == DISTANCE1) && !stopped && (round(error_signal(DISTANCE0)) >= cell_steps)) stop_early(); 

  ANGLE = angle_mode();
  if (prev_a == 10) prev_a = ANGLE;

  // Compute control signal (u(t))
  float ud = control_signal(DISTANCE, dt);
  float ua = control_signal(ANGLE, dt);
  float ut = control_signal(TURN, dt);

  // Drive motors using control signal
  if (dir == FORWARD) { // Drive straight
    // Distance value
    int pwr_d = (int)fabsf(ud);
    if (pwr_d > 255) pwr_d = 255;

    int ms = ((DISTANCE == DISTANCE0) ? MIN_SPEED : MIN_SPEED1);
    if (pwr_d < ms) {
      if ((DISTANCE == DISTANCE0) && (fabsf(error_signal(DISTANCE)) != 0.0)) pwr_d = ms;
      if ((DISTANCE == DISTANCE1) && (Sensors.f_threshold != Sensors.f_limit) && (fabsf(error_signal(DISTANCE)) > 5.0)) pwr_d = ms;
    }
    // Angle values
    //if (fabsf(ua) > 25.0) ua = (fabsf(ua)/ua)*25.0;
    int pwr_L = (pwr_d - round(ua));
    int pwr_R = (pwr_d + round(ua));
    if (pwr_L > 255) pwr_L = 255;
    if (pwr_R > 255) pwr_R = 255;
    if (pwr_L < 0) pwr_L = 0;
    if (pwr_R < 0) pwr_R = 0;
    // Make sure it moves
    //digitalWrite(Sensors.LED_R, (error_signal(ANGLE) == 0.0));
    //digitalWrite(LED_BUILTIN, (eprev[DISTANCE] == 0.0));
    if ((error_signal(ANGLE) < -1.0) && (pwr_L < ms)) pwr_L = ms;
    if ((error_signal(ANGLE) > 1.0) && (pwr_R < ms)) pwr_R = ms;
  
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
    if ((error_signal(TURN) != 0.0) && (pwrt < MIN_SPEED_TURN)) pwrt = MIN_SPEED_TURN;

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
  if (ANGLE != ANGLE0) enc_offset = (Encoders.getPosL() - Encoders.getPosR());
}

int pid_control::dir2mode(int dir) {
  if (dir == FORWARD) return DISTANCE;
  else return TURN;
}

int pid_control::distance_mode() {
  if (recentering) return DISTANCE0;
  if (target < 0) return DISTANCE0;
  rF = ((Sensors.read_sensor(Sensors.IR_FL) + Sensors.read_sensor(Sensors.IR_FR))/2);
  //rF = Sensors.read_sensor(Sensors.IR_FR);
  digitalWrite(LED_BUILTIN, (rF >= Sensors.f_limit));
  // No wall in front
  if (rF < Sensors.f_limit) return DISTANCE0;
  // Wall in front
  else return DISTANCE1;
}

int pid_control::angle_mode() {
  if (recentering) return ANGLE0;

  rL = Sensors.read_sensor(Sensors.IR_L);
  rR = Sensors.read_sensor(Sensors.IR_R);
  // Both detect a wall
  if ((rL > Sensors.limit) && (rR > Sensors.limit)) {
    prev_a = ANGLE2;
    return ANGLE2;
  }
  // Neither detect a wall
  else if ((rL <= Sensors.limit) && (rR <= Sensors.limit)) {
    prev_a = ANGLE0;
    return ANGLE0;
  }
  // XOR left sensor detects a wall, right sensor detects a wall
  else if ((rL > Sensors.limit) ^ (rR > Sensors.limit)) {
    // Dont clip a wall
    if (((prev_a == ANGLEL) && (rR > Sensors.limit)) || ((prev_a == ANGLER) && (rL > Sensors.limit)) || (prev_a == ANGLE0)) {
      if (!savet) {
        savet = 1;
        init_time = millis();
        timer = init_time;
        return ANGLE0;
      }
      else {
        timer = millis();
        if ((timer - init_time) > 50) savet = 0;
        else return ANGLE0;
      }
    }
    if (rL > Sensors.limit) prev_a = ANGLEL;
    if (rR > Sensors.limit) prev_a = ANGLER;
    return ANGLE1;
  }
  return ANGLE0;
}

// Undetected wall in path
void pid_control::stop_early() {
  stopped = 1;
  if (get_curr_cells() == 0) return;
  int temp = Sensors.f_threshold;
  Sensors.f_threshold = Sensors.f_limit;
  int ti = millis();
  int t = ti;
  while ((t - ti) < 1000) {
    control_loop(FORWARD);
    t = millis();
  }
  Sensors.f_threshold = temp;
}

pid_control PD = pid_control();