#ifndef HAND_H
#define HAND_H

class Hand{
private:
  Leap::Hand hand;
  //coordinatesForLDS convertToLDS(Leap coordinates);

public:
  std::vector<dirObject> objects;
  Hand();
  int getHandID();
  bool isRight();
  bool isLeft();
  //coordinatesForLDS getPinchLocation(); //get location of thumb in float32s
};

#endif
