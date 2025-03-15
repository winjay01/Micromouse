#ifndef pid_control_h
#define pid_control_h

struct pid_control {
  float kp[3], ki[3], kd[3];
  float eint[3], eprev[3];
  int prevT;
  // Targets
  int cell_steps, turn_diff;
  // Directions
  int FORWARD, LEFT, RIGHT;
  // Modes
  int DISTANCE, ANGLE, TURN;

  pid_control();
  void RESET();
  int error_signal(int mode);
  int control_signal(int mode, float dt);
  void control_loop(int dir);
  int dir2mode(int dir);
};

extern pid_control PD;

#endif