#include "GestureEvent.hpp"

GestureEvent::GestureEvent(std::string eventName, GestureType gType){
  name = eventName;
  type = gType;
}

std::string GestureEvent::getName(){
	return name;
}

GestureType GestureEvent::getGestureType(){
	return type;
}
