#ifndef pid_control_h
#define pid_control_h

struct pid_control {
  float kp[6], ki[6], kd[6];
  float eint[6], eprev[6];
  long prevT;
  // Targets
  int cell_steps, target, turn_diff;
  // Directions
  int FORWARD, LEFT, RIGHT;
  // Modes
  int DISTANCE0, ANGLE0, TURN, ANGLE1, ANGLE2, DISTANCE1;
  int ANGLE, DISTANCE; // Will be one of ANGLE0, ANGLE1, ANGLE2; DISTANCE0, DISTANCE1
  int pathfinding, recentering;

  pid_control();
  void set_target(int num_cells);
  void set_target_precise(int steps);
  void RESET();
  int get_curr_steps();
  int get_curr_cells();
  float error_signal(int mode);
  float control_signal(int mode, float dt);
  void control_loop(int dir);
  int dir2mode(int dir);
  int distance_mode();
  int angle_mode();
  void stop_early();
};

extern pid_control PD;

#endif