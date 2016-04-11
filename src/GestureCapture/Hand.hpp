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
  std::array<std::array<std::tuple<float,float,float>, 4>, 5> wow;

public:
  std::vector<DirObject> objects;
  Hand(Leap::Hand hand);
  ~Hand();
  int getHandID();
  bool isRight();
  bool isLeft();
  std::tuple<float,float,float> getPalmLocation();
  std::tuple<float,float,float> getFingerLocation(int fingerIndex);
  std::array<std::array<std::tuple<float,float,float>, 4>, 5> getFingers();
};

#endif
