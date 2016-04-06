#include "GHand.hpp"
#include "irrlicht/IMeshSceneNode.h"
#include <tuple>

GHand::GHand()
: node(nullptr)
{}

GHand::GHand(irr::scene::ISceneManager* smgr)
{
    node = smgr->addSphereSceneNode();
    node->setVisible(false);
    node->setPosition(irr::core::vector3df(-1,-1,-1));


node->getMaterial(0).SpecularColor.set(128, 255, 192, 203);
node->getMaterial(0).AmbientColor.set(128, 255, 192, 203);
node->getMaterial(0).DiffuseColor.set(128, 255, 192, 203);
node->getMaterial(0).EmissiveColor.set(0,0,0,0);
setXYZ(50,50,50);
}

GHand::~GHand()
{
    if(node) node->remove();
}

void GHand::setXYZ(irr::f32 x,irr::f32 y, irr::f32 z)
{
    irr::core::vector3df thing = irr::core::vector3df(x,y,z);
    node->setPosition(thing);
}
void GHand::setVisible(bool vis)
{
    node->setVisible(vis);
    if( false == vis )
    {
    node->setPosition(irr::core::vector3df(-1,-1,-1));
    }
}
void GHand::setTexture(irr::video::ITexture* texture)
{
}

void GHand::copyHand(Hand& hand)
{
    float x,y,z;
    std::tie(x,y,z) = hand.getPalmLocation();
    //node->setPosition(irr::core::vector3df(x*100+100,z*100+100,(1.5-y)*100-100));
    node->setPosition(irr::core::vector3df(25+50*x,-25+50*(y-1),25+50*(-z)));
    node->setVisible(true);

}
