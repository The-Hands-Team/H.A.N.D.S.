#include "GestureQueue.hpp"

void GestureQueue::push(const GestureEvent* ge)
{
	gesture_q.push(ge);
}

GestureEvent* GestureQueue::pop()
{
	GestureEvent* ret = gesture_q.front();
	gesture_q.pop();
	return ret;
}

GestureEvent* GestureQueue::peek()
{
	return gesture_q.front();
}

int GestureQueue::size()
{
	return gesture_q.size();
}
