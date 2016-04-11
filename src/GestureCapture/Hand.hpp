#ifndef HAND_H
#define HAND_H

#include "Graphics/DirObject.hpp"
#include "LeapMotion/Leap.h"
#include <tuple>
#include <vector>

class Hand{
private:
  bool isRightHand;
  int handID;
  float* box;
  float* palm;
  std::vector<std::array<float, 3> > fingers;

public:
  std::vector<DirObject> objects;
  Hand(Leap::Hand newHand);
  ~Hand();
  int getHandID();
  bool isRight();
  bool isLeft();
  std::tuple<float,float,float> getPalmLocation();
  std::tuple<float,float,float> getFingerLocation(int fingerIndex);
};

#endif
