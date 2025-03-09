#ifndef motors_h
#define motors_h

struct motors {
  motors();
  void SETUP();
  void setMotorR(int dir, int pwm_val);
  void setMotorL(int dir, int pwm_val);
  void stop();
  void turn_right(int speed);
  void turn_left(int speed);
  void forward(int speed);
  void reverse(int speed);
};

extern motors Motors;

#endif