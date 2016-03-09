#ifndef GESTUREQUEUE_H
#define GESTUREQUEUE_H

#include "Message.hpp"
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>

class GestureQueue {
private:
    std::queue<std::unique_ptr<Message>> gesture_q;
    std::mutex q_mutex;
    static GestureQueue* instance;
public:
    ~GestureQueue();
    GestureQueue();
    static GestureQueue* getInstance();
    std::unique_ptr<Message> pop();
    int size ();
    std::condition_variable event_cv;
    void push(std::unique_ptr<Message>);

    template<typename MESSAGE_T>
    void push(std::unique_ptr<MESSAGE_T>&&);
};

template<typename TO, typename FROM, class DEST=std::default_delete<FROM>>
std::unique_ptr<TO> dynamic_unique_move
    (
    std::unique_ptr<FROM, DEST> src
    )
{
    if(TO* result = dynamic_cast<TO*>(src.get())) {
        src.release();
        return std::unique_ptr<TO>(result);
    }
    return std::unique_ptr<TO>(nullptr);
}



template<typename MESSAGE_T>
void GestureQueue::push(std::unique_ptr<MESSAGE_T>&& ge)
{
    q_mutex.lock();
    gesture_q.emplace(dynamic_unique_move<Message, MESSAGE_T>(std::move(ge)));
    event_cv.notify_one();
    q_mutex.unlock();
}

#endif
