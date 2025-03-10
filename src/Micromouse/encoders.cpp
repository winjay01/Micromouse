#include <Arduino.h>
#include "encoders.h"

const int RENCA = 8;
const int RENCB = 7;
const int LENCA = 3;
const int LENCB = 4;

int cell_count = 10; // # of encoder counts needed to move forward 1 cell
int r_status, r_init_value;

// Interrupt Service Routines
void readL() {
  Encoders.readEncoderL();
}

void readR() {
  Encoders.readEncoderR();
}

ISR (PCINT0_vect) {
  r_status = digitalRead(RENCA);
  // Only call ISR on rising
  if (((r_init_value == 0) && (r_status == 0)) || ((r_init_value == 1) && (r_status == 1))) readR();
}

encoders::encoders() {
  posL = 0;
  posR = 0;
}

void encoders::SETUP() {
  Serial.begin(9600);
  pinMode(RENCA, INPUT);
  pinMode(RENCB, INPUT);
  pinMode(LENCA, INPUT);
  pinMode(LENCB, INPUT);
  attachInterrupt(digitalPinToInterrupt(LENCA), readL, RISING);
  //attachInterrupt(digitalPinToInterrupt(RENCA), readR, RISING);

  // Enable pin change interrupt for pin 8 (RENCA)
  PCICR |= B00000001;
  PCMSK0 = 1 << PCINT0;
  Serial.println("BEGIN: RENCA is " + (String)digitalRead(RENCA) + " LENCA is " + (String)digitalRead(LENCA));
  r_init_value = digitalRead(RENCA);
}

void encoders::readEncoderR() {
  int b = digitalRead(RENCB);
  if (b > 0) {
    if (r_init_value == 0) posR++;
    else posR--;
  }
  else {
    if (r_init_value == 0) posR--;
    else posR++;
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
  //Serial.println(posR);
  return posR;
}

int encoders::getPosL() {
  //Serial.println(posL);
  return posL;
}

encoders Encoders = encoders();