#include <Arduino.h>
#include "encoders.h"

// Interrupt Service Routines
void readL() {
  Encoders.readEncoderL();
}

void readR() {
  Encoders.readEncoderR();
}

encoders::encoders() {
  LENCA = 2;
  LENCB = 7;
  RENCA = 3;
  RENCB = 4;
  posL = 0;
  posR = 0;
}

void encoders::SETUP() {
  pinMode(RENCA, INPUT);
  pinMode(RENCB, INPUT);
  pinMode(LENCA, INPUT);
  pinMode(LENCB, INPUT);
  attachInterrupt(digitalPinToInterrupt(LENCA), readL, RISING);
  attachInterrupt(digitalPinToInterrupt(RENCA), readR, RISING);
}

void encoders::RESET() {
  posL = 0;
  posR = 0;
}

void encoders::readEncoderR() {
  int b = digitalRead(RENCB);
  if (b > 0) {
    posR--;
  }
  else {
    posR++;
  }
}

void encoders::readEncoderL() {
  int b = digitalRead(LENCB);
  if (b > 0) {
    posL++;
  }
  else {
    posL--;
  }
}

int encoders::getPosR() {
  return posR;
}

int encoders::getPosL() {
  return posL;
}

encoders Encoders = encoders();

/* 
int r_status, r_init_value;
  // Enable pin change interrupt for pin 8 (RENCA)
  PCICR |= B00000001;
  PCMSK0 = 1 << PCINT0;
  Serial.println("BEGIN: RENCA is " + (String)digitalRead(RENCA) + " LENCA is " + (String)digitalRead(LENCA));

ISR (PCINT0_vect) {
  r_status = digitalRead(RENCA);
  // Only call ISR on rising
  if (((r_init_value == 0) && (r_status == 0)) || ((r_init_value == 1) && (r_status == 1))) readR();
}
*/