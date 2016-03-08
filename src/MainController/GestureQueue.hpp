#ifndef GESTUREQUEUE_H
#define GESTUREQUEUE_H

#include "Message.hpp"
#include <mutex>
#include <queue>

class GestureQueue {
private:
    std::queue<Message*> gesture_q;
    std::mutex q_mutex;
    static GestureQueue* instance;
public:
    ~GestureQueue();
    GestureQueue();
    static GestureQueue* getInstance();
    void push (Message*);
    Message* pop ();
    Message* peek ();
    int size ();
    std::condition_variable event_cv;
};

#endif
