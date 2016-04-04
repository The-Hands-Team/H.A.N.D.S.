#include "Hand.hpp"

Hand::Hand(Leap::Hand newHand)
:hand(newHand)
{
}

int Hand::getHandID(){
    if(hand.isValid())
        return hand.id();

    return -1;
}

bool Hand::isRight(){
    if(hand.isValid())
        return hand.isRight();

    return false;
}

bool Hand::isLeft(){
    if(hand.isValid())
        return hand.isLeft();

    return false;
}

std::tuple<float,float,float> Hand::getPalmLocation(){
    if(!hand.isValid()) return std::make_tuple(-1,-1,-1);
  Leap::Vector palmPos = hand.palmPosition();
  Leap::InteractionBox iBox = hand.frame().interactionBox();
  return std::make_tuple(palmPos.x/iBox.width(), palmPos.y/iBox.height(), palmPos.z/iBox.depth());
}

std::tuple<float,float,float> Hand::getFingerLocation(int fingerIndex){
if(!hand.isValid()) return std::make_tuple(-1,-1,-1);
  Leap::FingerList fingers = hand.fingers();
  if(fingerIndex>fingers.count())
    return std::make_tuple(-1,-1,-1);
  Leap::Vector fingerPos = fingers[fingerIndex].tipPosition();
  Leap::InteractionBox iBox = hand.frame().interactionBox();
  return std::make_tuple(fingerPos.x/iBox.width(), fingerPos.y/iBox.height(), fingerPos.z/iBox.depth());
}
