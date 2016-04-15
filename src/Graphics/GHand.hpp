#ifndef GHAND_HPP
#define GHAND_HPP

#include <tuple>

#include "irrlicht/ISceneManager.h"
#include "irrlicht/ISceneNode.h"
#include "GestureCapture/Hand.hpp"

class GHand
{
public:
    GHand();
    void init(irr::scene::ISceneManager*);
    ~GHand();
    void setXYZ(float x,float y, float z);
    std::tuple<float,float,float> getXYZ();
    void copyHand(Hand& h);
    void setVisible(bool vis);
    void setTexture(irr::video::ITexture*);
private:

    irr::scene::ISceneNode* node;
};

#endif
