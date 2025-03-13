#ifndef pid_control_h
#define pid_control_h

struct pid_control {
  float kp[3], ki[3], kd[3];

  pid_control();
  void SETUP(float KP[3], float KI[3], float KD[3]);
  int control_signal(int mode, float dt);
  void control_loop();
};

extern pid_control PD;

#endif