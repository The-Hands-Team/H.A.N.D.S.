#include "Hand.hpp"

Hand::Hand(Leap::Hand hand){
  if(!hand.isValid())
    return;
	
  isRightHand = hand.isRight();
  handID = hand.id();
  
  Leap::InteractionBox iBox = hand.frame().interactionBox();
  box = new float[3];
  box[0] = iBox.width();
  box[1] = iBox.height();
  box[2] = iBox.depth();
  
  Leap::Vector palmPos = hand.palmPosition();
  palm = new float[3];
  palm[0] = palmPos.x;
  palm[1] = palmPos.y;
  palm[2] = palmPos.z;
  
  int i = 0;
  Leap::FingerList allTheFingers = hand.frame().fingers();
  for(Leap::FingerList::const_iterator fl=allTheFingers.begin(); fl!=allTheFingers.end(); fl++)
  {
    Leap::Vector fingerPos = (*fl).tipPosition();
	  fingers[i][0] = fingerPos.x;
    fingers[i][1] = fingerPos.y;
    fingers[i][2] = fingerPos.z;
	  i++;
  }
}

int Hand::getHandID(){
  return handID;
}

bool Hand::isRight(){
  return isRightHand;
}

bool Hand::isLeft(){
  return !isRightHand;
}

std::tuple<float,float,float> Hand::getPalmLocation(){
  return std::make_tuple(palm[0]/box[0], palm[1]/box[1], palm[2]/box[2]);
}

std::tuple<float,float,float> Hand::getFingerLocation(int fingerIndex){
  return std::make_tuple(fingers[fingerIndex][0]/box[0], fingers[fingerIndex][1]/box[1], fingers[fingerIndex][2]/box[2]);
}

Hand::~Hand(){
  if(box)
    delete[] box;
  if(palm)
    delete[] palm;
}
