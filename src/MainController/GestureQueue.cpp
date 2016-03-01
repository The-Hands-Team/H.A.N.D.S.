#include "GestureQueue.hpp"
#include <iostream>

GestureQueue* GestureQueue::instance = nullptr;

void GestureQueue::push(Message* ge)
{
    q_mutex.lock();
    gesture_q.push(ge);
    q_mutex.unlock();
    event_cv.notify_one();
}

Message* GestureQueue::pop()
{
    std::unique_lock<std::mutex> lk(event_m);
    event_cv.wait(lk, [&]{return gesture_q.size() != 0;});
    Message* ret = gesture_q.front();
    gesture_q.pop();
    lk.unlock();
    return ret;

}

Message* GestureQueue::peek()
{
    Message* ret = nullptr;
    q_mutex.lock();
    if( 0 != gesture_q.size())
        ret =  gesture_q.front();
    q_mutex.unlock();
    return ret;
}

int GestureQueue::size()
{
    git q_mutex.lock();
    int ret =  gesture_q.size();
    q_mutex.unlock();
    return ret;
}

GestureQueue* GestureQueue::getInstance()
{
    if( nullptr == GestureQueue::instance )
    {
        GestureQueue::instance = new GestureQueue();
    }
    return GestureQueue::instance;
}

GestureQueue::~GestureQueue()
{
    GestureQueue::instance = nullptr;
}
