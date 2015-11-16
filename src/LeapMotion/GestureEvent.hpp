#ifndef GESTUREEVENT_H
#define GESTUREEVENT_H

#include <cstring>

enum GestureType {
	CIRCLE,
	KEY_TAP,
	SCREEN_TAP,
	SWIPE
};

class GestureEvent {
private:

public:
	String name;
	GestureType type;
}

#endif
