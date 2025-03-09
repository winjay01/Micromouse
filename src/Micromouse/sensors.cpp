#include "Arduino.h"
#include "sensors.h"

const int IR1 = A0;
const int IR2 = A2;
const int IR3 = A4;
const int IR4 = A6;
int reading;

sensors::sensors() {
}

void sensors::SETUP() {
}

void sensors::read_sensor(int sensor_num) {
  // read IR sensor
  reading = analogRead(sensor_num);
  //Serial.println(reading);
  if (reading > 150) {
    digitalWrite(LED_BUILTIN, HIGH);
    return;
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
    return;
  }
}

void sensors::idle(int timer) {
  for (int i = 0; i < timer; i++) {
    read_sensor(IR4);
    delay(1);
  }
}

sensors Sensors = sensors();