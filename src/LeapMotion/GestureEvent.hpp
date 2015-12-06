#ifndef GESTUREEVENT_H
#define GESTUREEVENT_H
#include <string>

enum GestureType {
	CIRCLE,
	KEY_TAP,
	SCREEN_TAP,
	SWIPE_UP,
	SWIPE_DOWN,
	SWIPE_RIGHT,
	SWIPE_LEFT,
	INVALID_GESTURE
};

class GestureEvent {
private:
	std::string name;
	GestureType type;
	
public:
	GestureEvent(std::string eventName, GestureType gType);
	std::string getName();
	GestureType getGestureType();
};

#endif
