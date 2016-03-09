#include "GestureQueue.hpp"
#include <iostream>

GestureQueue* GestureQueue::instance = nullptr;


void GestureQueue::push(std::unique_ptr<Message> ge)
{
    q_mutex.lock();
    gesture_q.emplace(std::move(ge));
    event_cv.notify_one();
    q_mutex.unlock();
}

std::unique_ptr<Message> GestureQueue::pop()
{
    std::unique_lock<std::mutex> lk(q_mutex);
    event_cv.wait(lk, [&]{return gesture_q.size() != 0;});
    std::unique_ptr<Message> ret = std::move(gesture_q.front());
    gesture_q.pop();
    event_cv.notify_all();
    lk.unlock();
    return ret;

}

int GestureQueue::size()
{
    q_mutex.lock();
    int ret =  gesture_q.size();
    q_mutex.unlock();
    return ret;
}

GestureQueue::GestureQueue()
{
    instance = this;
}

GestureQueue* GestureQueue::getInstance()
{
    return instance;
}

GestureQueue::~GestureQueue()
{
    GestureQueue::instance = nullptr;
}
