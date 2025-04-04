#include "Arduino.h"
#include "sensors.h"

// Robot is 11.5 cm long, 10.5 cm wide
// So front threshold is 3.25 cm,
// Side threshold is 3.75 cm

sensors::sensors() {
  IR_R = A0; // Right
  IR_FR = A2; // Front Right
  IR_FL = A4; // Front Left
  IR_L = A6; // Left
  LED_L = 8;
  LED_R = 12;
  TIMER = 20;
  limit = 170;
  threshold = 260; // 3.75 cm
  f_threshold = 290; // 3.25 cm
  f_limit = 120;
}

void sensors::SETUP() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_L, OUTPUT);
  pinMode(LED_R, OUTPUT);
}

int sensors::read_all() {
  int bitmap = 0;
  int fl = read_sensor(IR_FL);
  int fr = read_sensor(IR_FR);
  if (read_sensor(IR_L) > limit) bitmap += 2;
  if (read_sensor(IR_R) > limit) bitmap += 4;
  if (fl > f_limit) bitmap += 8;
  if (fr > f_limit) bitmap += 1;
  return bitmap;
}

int sensors::read_front() {
  int front = ((read_sensor(IR_FL) > f_threshold) && (read_sensor(IR_FR) > f_threshold));
  digitalWrite(LED_L, front);
  return front;
}

int sensors::read_sensor(int sensor_num) {
  int led_pin = get_pin(sensor_num);
  int reading = analogRead(sensor_num);
  if (led_pin != LED_BUILTIN) digitalWrite(led_pin, (reading > limit));
  return reading;
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

void sensors::leds_off() {
  digitalWrite(LED_L, LOW);
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_BUILTIN, LOW);
}

sensors Sensors = sensors();