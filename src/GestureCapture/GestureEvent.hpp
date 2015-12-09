#ifndef GESTUREEVENT_H
#define GESTUREEVENT_H
#include <string>
#include <condition_variable>


void initGesture(bool);

const std::string gestureNames[] = {"Circle", "Key Tap", "Screen Tap", "Swipe Up", "Swipe Down", "Swipe Right", "Swipe Left"};

//enum GestureType {
//	CIRCLE,
//	KEY_TAP,
//	SCREEN_TAP,
//	SWIPE_UP,
//	SWIPE_DOWN,
//	SWIPE_RIGHT,
//	SWIPE_LEFT,
//	INVALID_GESTURE
//};

//class GestureEvent {
//private:
//	std::string name;
//	GestureType type;
//
//public:
//	GestureEvent(std::string eventName, GestureType gType);
//	std::string getName();
//	GestureType getGestureType();
//};

#endif
