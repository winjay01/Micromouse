#ifndef sensors_h
#define sensors_h

struct sensors {
  int IR_L, IR_R, IR_FL, IR_FR;
  int LED_L, LED_R;
  int TIMER;
  int threshold, f_threshold, f_limit;

  sensors();
  void SETUP();
  int read_sensor(int sensor_num);
  int read_front();
  int read_all();
  void blink(int led_pin, int del, int count);
  int get_pin(int sensor_num);
};

extern sensors Sensors;

#endif