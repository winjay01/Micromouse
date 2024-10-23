#include<iostream>
using namespace std;

template<class itemType>
class QueueInterface {
    public:
    virtual bool empty() const = 0;
    virtual bool enQueue(const itemType newEntry) = 0;
    virtual bool deQueue() = 0;
    virtual itemType front() const = 0;
    virtual int size() const = 0;
};

const int MAX_QUEUE = 1000;

template<class itemType>
class arrayQueue : public QueueInterface<itemType> {
    private:
    int head, tail;
    itemType items[MAX_QUEUE];

    public:
    arrayQueue();
    bool empty() const;
    bool enQueue(const itemType newEntry);
    bool deQueue();
    itemType front() const;
    int size() const;
};

template<class T>
arrayQueue<T>::arrayQueue() {
    head = -1;
    tail = -1;
}

template<class T>
bool arrayQueue<T>::empty() const {
    if (head == -1) return true;
    else return false;
}
// Wraparound implementation
template<class T>
bool arrayQueue<T>::enQueue(const T newEntry) {
    if (tail == (MAX_QUEUE - 1)) {
        if (head > 0) tail = 0;
        else return false;
    }
    else if (tail + 1 == head) return false;
    else tail++;
    items[tail] = newEntry;
    if (head == -1) head = 0;
    return true;
}

template<class T>
bool arrayQueue<T>::deQueue() {
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

template<class T>
T arrayQueue<T>::front() const {
    if (head == -1) throw exception();
    return items[head];
}

template<class T>
int arrayQueue<T>::size() const {
    if (head == -1) return 0;
    if (tail < head) return (MAX_QUEUE - head) + (tail + 1);
    return (tail - head) + 1;
}