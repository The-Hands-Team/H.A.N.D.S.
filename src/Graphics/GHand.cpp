#include "GHand.hpp"
#include "GraphicsConsts.hpp"
#include "irrlicht/IMeshSceneNode.h"
#include <tuple>

GHand::GHand()
: node(nullptr)
{}

void GHand::init(irr::scene::ISceneManager* smgr)
{
    node = smgr->addSphereSceneNode(GraphicsConsts::CELL_WIDTH/2.0);
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
    if(node)
	    node->remove();
}

void GHand::setXYZ(irr::f32 x,irr::f32 y, irr::f32 z)
{
	if(node)
        node->setPosition(irr::core::vector3df(x,y,z));
}
std::tuple<float,float,float> GHand::getXYZ()
{
    irr::core::vector3df pos(-1,-1,-1);

    if(node)
    {
        pos = node->getPosition();
    }

    return std::make_tuple( pos.X, pos.Y, pos.Z );
}
void GHand::setVisible(bool vis)
{
	if(node)
	{
		node->setVisible(vis);
		if( false == vis )
		{
		node->setPosition(irr::core::vector3df(-1,-1,-1));
		}
	}
}
void GHand::setTexture(irr::video::ITexture* texture)
{
}

void GHand::copyHand(Hand& hand)
{
	if(node)
	{
		float x,y,z;
		std::tie(x,y,z) = hand.getPalmLocation();
		//node->setPosition(irr::core::vector3df(x*100+100,z*100+100,(1.5-y)*100-100));
		node->setPosition(irr::core::vector3df(GraphicsConsts::VIEW_WIDTH/2.0+50*x,-GraphicsConsts::VIEW_HEIGHT/2.0+50*(y-1),GraphicsConsts::CAM_HEIGHT+50*(-z)));
		node->setVisible(true);
	}

}
