#include "Arduino.h"
#include "sensors.h"

const int LED_L = 8;
const int LED_R = 12;
int reading;

sensors::sensors() {
  IR_R = A0; // Right
  IR_FR = A2; // Front Right
  IR_FL = A4; // Front Left
  IR_L = A6; // Left
  TIMER = 20;
}

void sensors::SETUP() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_L, OUTPUT);
  pinMode(LED_R, OUTPUT);
}

int sensors::read_all() {
  int bitmap = 0;
  int front = 0;
  if (read_sensor(IR_L)) bitmap += 1;
  if (read_sensor(IR_R)) bitmap += 2;
  if (read_sensor(IR_FL)) {
    bitmap += 4;
    front = 1;
  }
  if (read_sensor(IR_FR)) {
    bitmap += 8;
    front = 1;
  }
  digitalWrite(LED_BUILTIN, front);
  return bitmap;
}

int sensors::read_sensor(int sensor_num) {
  int led_pin = get_pin(sensor_num);
  // read IR sensor
  reading = analogRead(sensor_num);
  //Serial.println(reading);
  if (reading > 150) {
    if (led_pin != LED_BUILTIN) digitalWrite(led_pin, HIGH);
    return 1;
  }
  else {
    if (led_pin != LED_BUILTIN) digitalWrite(led_pin, LOW);
    return 0;
  }
}

void sensors::blink(int led_pin, int del, int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(led_pin, HIGH);
    delay(del);
    digitalWrite(led_pin, LOW);
    delay(del);
  }
}

int sensors::get_pin(int sensor_num) {
  int led_pin;
  if ((sensor_num == IR_FL) || (sensor_num == IR_FR)) led_pin = LED_BUILTIN;
  else if (sensor_num == IR_L) led_pin = LED_L;
  else if (sensor_num = IR_R) led_pin = LED_R; 
  return led_pin;
}

sensors Sensors = sensors();