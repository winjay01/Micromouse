#ifndef motors_h
#define motors_h

struct motors {
  int F, B, L, R;

  motors();
  void SETUP();
  void setMotorR(int dir, int pwm_val);
  void setMotorL(int dir, int pwm_val);
  void stop();
  void turn_right(int speed);
  void turn_left(int speed);
  void forward(int speed);
  void reverse(int speed);
  void drive(int dir, int speed);
  void drive_pos(int enc_steps, int dir, int speed);
  void drive_for(int time, int dir, int speed);
};

extern motors Motors;

#endif