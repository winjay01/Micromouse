#ifndef pid_control_h
#define pid_control_h

struct pid_control {
  float kp, ki, kd;
  float u;

  pid_control();
  void SETUP(int kp, int ki, int kd);
  int pd_control(int kp, int kd, int curr_time, int prev_time);
  void control_loop();
};

extern pid_control PD;

#endif