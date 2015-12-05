#include "GestureQueue.hpp"

void GestureQueue::push(GestureEvent* ge)
{
	q_mutex.lock();
	gesture_q.push(ge);
	q_mutex.unlock();
}

GestureEvent* GestureQueue::pop()
{
	q_mutex.lock();
	GestureEvent* ret = gesture_q.front();
	gesture_q.pop();
	q_mutex.unlock();
	return ret;

}

GestureEvent* GestureQueue::peek()
{
	q_mutex.lock();
	GestureEvent* ret =  gesture_q.front();
	q_mutex.unlock();
	return ret;
}

int GestureQueue::size()
{
	q_mutex.lock();
	int ret =  gesture_q.size();
	q_mutex.unlock();
	return ret;
}
