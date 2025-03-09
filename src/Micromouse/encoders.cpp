#include <Arduino.h>
#include "encoders.h"

const int RENCA = 8;
const int RENCB = 7;
const int LENCA = 3;
const int LENCB = 4;

int cell_count = 10; // # of encoder counts needed to move forward 1 cell
int posL = 0; // encoder count for left motor
int posR = 0; // encoder count for right motor
int r_status, r_init_value;

// Interrupts
void readL() {
  Encoders.readEncoderL();
}

void readR() {
  Encoders.readEncoderR();
}

ISR (PCINT0_vect) {
  r_status = digitalRead(RENCA);
  if (r_init_value == 0) {
    // Rising
    if (r_status == 0) {
      readR();
    }
  }
  else {
    if (r_status == 1) {
      readR();
    }
  }
}

encoders::encoders() {
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
  //Serial.println("RENCB: " + (String)b + " R Status: " + (String)r_status + " PosR: " + (String)posR);
}

void encoders::readEncoderL() {
  int b = digitalRead(LENCB);
  if (b > 0) {
    posL++;
  }
  else {
    posL--;
  }
  //Serial.println("LENCB: " + (String)b + " PosL: " + (String)posL);
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