#include "Hand.hpp"

Hand::Hand(Leap::Hand newHand){
  hand = newHand;
}

int Hand::getHandID(){
  return hand.id();
}

bool Hand::isRight(){
  return hand.isRight();
}

bool Hand::isLeft(){
  return hand.isLeft();
}

std::tuple<float,float,float> Hand::getPalmLocation(){
  Leap::Vector palmPos = hand.palmPosition();
  Leap::InteractionBox iBox = hand.frame().interactionBox();
  return std::make_tuple(palmPos.x/iBox.width(), palmPos.y/iBox.height(), palmPos.z/iBox.depth());
}
