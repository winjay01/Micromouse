#ifndef encoders_h
#define encoders_h

struct encoders {
  int posL; // encoder count for left motor
  int posR; // encoder count for right motor
  
  encoders();
  void SETUP();
  void readEncoderR();
  void readEncoderL();
  int getPosR();
  int getPosL();
};

extern encoders Encoders;

#endif