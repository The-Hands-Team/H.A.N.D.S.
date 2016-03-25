#ifndef HAND_H
#define HAND_H

class Hand{
private:
  Leap::Hand hand;

public:
  std::vector<dirObject> objects;
  Hand(Leap::Hand newHand);
  int getHandID();
  bool isRight();
  bool isLeft();
	float* getPalmLocation();
};

#endif
