#ifndef HAND_H
#define HAND_H

#include "Graphics/DirObject.hpp"
#include "LeapMotion/Leap.h"
#include <tuple>
#include <vector>

class Hand{
private:
  Leap::Hand hand;

public:
  std::vector<DirObject> objects;
  Hand(Leap::Hand newHand);
  int getHandID();
  bool isRight();
  bool isLeft();
  std::tuple<float,float,float> getPalmLocation();
  std::tuple<float,float,float> getFingerLocation(int fingerIndex);
};

#endif
