#ifndef queue_h
#define queue_h

struct queue {
  int MAX_QUEUE;
  int head, tail;
  int items[64];

  queue();
  bool empty();
  bool enqueue(int entry);
  bool dequeue();
  int front();
};

extern queue q;

#endif