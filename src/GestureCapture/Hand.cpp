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

std::tuple<float,float,float> Hand::getFingerLocation(int fingerIndex){
  Leap::FingerList fingers = hand.fingers();
  if(fingerIndex>fingers.count())
    return NULL;
  Leap::Vector fingerPos = fingers[fingerIndex].tipPosition();
  Leap::InteractionBox iBox = hand.frame().interactionBox();
  return std::make_tuple(fingerPos.x/iBox.width(), fingerPos.y/iBox.height(), fingerPos.z/iBox.depth());
}
