#include "Hand.hpp"

Hand::Hand( const Leap::Hand& hand){
  if(!hand.isValid())
    return;
	
  isRightHand = hand.isRight();
  handID = hand.id();
  
  Leap::InteractionBox iBox = hand.frame().interactionBox();
  box[0] = iBox.width();
  box[1] = iBox.height();
  box[2] = iBox.depth();
  
  Leap::Vector palmPos = hand.palmPosition();
  palm = std::make_tuple(palmPos.x, palmPos.y, palmPos.z);
  
  int f = 0;
  Leap::FingerList allTheFingers = hand.frame().fingers();
  for(Leap::FingerList::const_iterator fl=allTheFingers.begin(); fl!=allTheFingers.end(); fl++)
  {
    Leap::Vector fingerPos = (*fl).tipPosition();
    fingers.push_back( std::array<float, 3>{fingerPos.x, fingerPos.y,  fingerPos.z} );
    
    Leap::Bone bone;
    Leap::Bone::Type boneType;
    for(int b=0; b<4; b++)
    {
      boneType = static_cast<Leap::Bone::Type>(b);
      bone = (*fl).bone(boneType);
      Leap::Vector boneEnd = bone.nextJoint();
      wow[f][b] = std::make_tuple(boneEnd.x, boneEnd.y, boneEnd.z);
    }
    f++;
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
    float palmX, palmY, palmZ;
    std::tie( palmX, palmY, palmZ ) = palm;
  return std::make_tuple(palmX/box[0], palmY/box[1], palmZ/box[2]);
}

std::tuple<float,float,float> Hand::getFingerLocation(int fingerIndex){
  return std::make_tuple(fingers[fingerIndex][0]/box[0], fingers[fingerIndex][1]/box[1], fingers[fingerIndex][2]/box[2]);
}


std::array<std::array<std::tuple<float,float,float>, 4>, 5> Hand::getFingers(){
  std::array<std::array<std::tuple<float,float,float>, 4>, 5> fings;
  for(int f=0; f<5; f++)
    for(int b=0; b<4; b++)
    {
    	float x,y,z;
    	std::tie(x,y,z) = wow[f][b];
      fings[f][b] = std::make_tuple(x/box[0], y/box[1], z/box[2]);
    }
  return std::move(fings);
}

Hand::~Hand(){
}
