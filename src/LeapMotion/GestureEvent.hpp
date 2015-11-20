#ifndef GESTUREEVENT_H
#define GESTUREEVENT_H

enum GestureType {
	CIRCLE,
	KEY_TAP,
	SCREEN_TAP,
	SWIPE
};

class GestureEvent {
private:

public:
	std::string name;
	GestureType type;
	
	GestureEvent(std::string eventName, GestureType gType);
};

#endif
