#ifndef encoders_h
#define encoders_h

struct encoders {
  int LENCA, LENCB, RENCA, RENCB;
  int posL; // encoder count for left motor
  int posR; // encoder count for right motor

  encoders();
  void SETUP();
  void RESET();
  void readEncoderR();
  void readEncoderL();
  int getPosR();
  int getPosL();
};

extern encoders Encoders;

#endif