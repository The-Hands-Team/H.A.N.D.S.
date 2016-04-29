#ifndef GHAND_HPP
#define GHAND_HPP

#include <tuple>

#include "irrlicht/ISceneManager.h"
#include "irrlicht/ISceneNode.h"
#include "irrlicht/IGUIEnvironment.h"
#include "GestureCapture/Hand.hpp"

using gridcoord = std::tuple<int,int,int>;

class GHand
{
public:
    GHand();
    void init(irr::scene::ISceneManager*);
    void deinit();
    ~GHand();
    void setXYZ(float x,float y, float z);
    std::tuple<float,float,float> getXYZ();
    void copyHand(Hand& h);
    void setVisible(bool vis);
    void pickUp(DirObject& target, gridcoord, irr::scene::ISceneManager*, irr::gui::IGUIEnvironment*);
    gridcoord dropObj();
private:

    irr::scene::IMeshSceneNode* palm;
    std::array<std::array<irr::scene::IMeshSceneNode*,7>,5> fingers;
    irr::scene::IMeshSceneNode* selector;
    irr::scene::IMeshSceneNode* heldNode;
    DirObject* heldObj;
    gridcoord heldObjLoc;
};

#endif
