#ifndef motors_h
#define motors_h

struct motors {
  int FORWARD, REVERSE;
  int FW, B, L, R;
  int LPWM1, LPWM2, RPWM1, RPWM2; // Motor +,- pins

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