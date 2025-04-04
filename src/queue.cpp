#include "queue.h"
#include <Arduino.h>

byte items[32];

queue::queue() {
  MAX_QUEUE = 32;
  head = -1;
  tail = -1;
}

bool queue::empty() {
  if (head == -1) return true;
  else return false;
}

bool queue::enqueue(int entry) {
  if (tail == (MAX_QUEUE - 1)) {
    if (head > 0) tail = 0;
    else return false;
  }
  else if (tail + 1 == head) return false;
  else tail++;
  items[tail] = entry;
  if (head == -1) head = 0;
  return true;
}

bool queue::dequeue() {
  if (head == -1) return false;
  if (head == tail) {
    head = -1;
    tail = -1;
    return true;
  }
  if (head == (MAX_QUEUE - 1)) {
    head = 0;
  }
  else head++;
  return true;
}

int queue::front() {
  if (head == -1) return -1;
  return items[head];
}

queue q = queue();