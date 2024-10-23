#include "myQueue.hpp"
#include <iostream>
using namespace std;

int main(){
    arrayQueue<int> Qobj;
    Qobj.enQueue(1);
    Qobj.enQueue(2);
    Qobj.enQueue(3);
    Qobj.enQueue(4);

    cout << Qobj.front() << endl;

    Qobj.deQueue();

    cout << Qobj.front() << endl;
    cout << Qobj.size() << endl;

}