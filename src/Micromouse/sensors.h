#ifndef sensors_h
#define sensors_h

struct sensors {
  sensors();
  void SETUP();
  void read_sensor(int sensor_num);
  void idle(int timer);
};

extern sensors Sensors;

#endif