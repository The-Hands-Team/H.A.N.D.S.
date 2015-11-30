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
	std::string name;
	GestureType type;
	
public:
	GestureEvent(std::string eventName, GestureType gType);
};

#endif
