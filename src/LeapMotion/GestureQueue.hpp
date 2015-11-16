#ifndef GESTUREQUEUE_H
#define GESTUREQUEUE_H
#include "GestureEvent.hpp"
#include <mutex>
class GestureQueue {
private:
	std::queue<GestureEvent*> gesture_q;
	std::mutex q_mutex;
public:
	void push (const GestureEvent*);
	GestureEvent* pop ();
	GestureEvent* peek ();
	int size ();
}

#endif
