#ifndef encoders_h
#define encoders_h

struct encoders {
  encoders();
  void SETUP();
  void readEncoderR();
  void readEncoderL();
  int getPosR();
  int getPosL();
};

extern encoders Encoders;

#endif