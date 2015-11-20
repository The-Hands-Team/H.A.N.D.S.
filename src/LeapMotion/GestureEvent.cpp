#include "GestureEvent.hpp"

GestureEvent::GestureEvent(std::string eventName, GestureType gType){
  name = eventName;
  type = gType;
  std::cout<<name<<"\t"<<type<<std::endl;
}
