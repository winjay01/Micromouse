#ifndef queue_h
#define queue_h

struct queue {
  int MAX_QUEUE;
  int head, tail;

  queue();
  bool empty();
  bool enqueue(int entry);
  bool dequeue();
  int front();
};

extern queue q;

#endif