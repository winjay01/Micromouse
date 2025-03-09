#ifndef queue_h
#define queue_h

struct queue {
  int head, tail;
  int items[1000];

  queue();
  bool empty();
  bool enqueue(int entry);
  bool dequeue();
  int front();
  int size();
};

extern queue q;

#endif