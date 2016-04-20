#include "GHand.hpp"
#include "Graphics.hpp"
#include "irrlicht/IMeshSceneNode.h"

#include <tuple>

GHand::GHand()
: palm(nullptr)
, fingers({{nullptr}})
{}

void GHand::init(irr::scene::ISceneManager* smgr)
{
    palm = smgr->addSphereSceneNode(Graphics::CELL_WIDTH/2.0);
    palm->setVisible(false);
    palm->setPosition(irr::core::vector3df(-1,-1,-1));

    for(int i = 0; i < 5; i++)
    {
       fingers[i][0] = smgr->addSphereSceneNode(Graphics::CELL_WIDTH/4.0);
       for(int j = 0; j < 3; j++)
       {
          irr::scene::IMesh* mesh = smgr->getGeometryCreator()->createCylinderMesh(
                                                                   1,
                                                                   1,
                                                                   10);
          fingers[i][j*2+1] = smgr->addMeshSceneNode(mesh);
          fingers[i][j*2+1]->setVisible(false);
          fingers[i][j*2+1]->setPosition(irr::core::vector3df(-1,-1,-1));
          fingers[i][j*2+2] = smgr->addSphereSceneNode(Graphics::CELL_WIDTH/4.0);
          fingers[i][j*2+2]->setVisible(false);
          fingers[i][j*2+2]->setPosition(irr::core::vector3df(-1,-1,-1));
       }
    }


palm->getMaterial(0).SpecularColor.set(128, 255, 192, 203);
palm->getMaterial(0).AmbientColor.set(128, 255, 192, 203);
palm->getMaterial(0).DiffuseColor.set(128, 255, 192, 203);
palm->getMaterial(0).EmissiveColor.set(0,0,0,0);
setXYZ(50,50,50);
}

GHand::~GHand()
{
    // Not remove(). If we get here the scene manager is dead.
    if( palm )
        palm->drop();
    for(std::array<irr::scene::ISceneNode*, 7> finger : fingers)
    {
        for(irr::scene::ISceneNode* node : finger)
        {
            if(node)
                node->drop();
        }
    }
}

void GHand::setXYZ(irr::f32 x,irr::f32 y, irr::f32 z)
{
    if(palm)
        palm->setPosition(irr::core::vector3df(x,y,z));
}
std::tuple<float,float,float> GHand::getXYZ()
{
    irr::core::vector3df pos(-1,-1,-1);

    if(palm)
    {
        pos = palm->getPosition();
    }

    return std::make_tuple( pos.X, pos.Y, pos.Z );
}
void GHand::setVisible(bool vis)
{
    if(palm)
    {
        palm->setVisible(vis);
        if( false == vis )
        {
        palm->setPosition(irr::core::vector3df(-1,-1,-1));
        }
    }
}
void GHand::setTexture(irr::video::ITexture* texture)
{
}

void GHand::copyHand(Hand& hand)
{
    if(palm)
    {
        float x,y,z;
        std::tie(x,y,z) = hand.getPalmLocation();
        //palm->setPosition(irr::core::vector3df(x*100+100,z*100+100,(1.5-y)*100-100));
        palm->setPosition(irr::core::vector3df(Graphics::VIEW_WIDTH/2.0+50*x,-Graphics::VIEW_HEIGHT/2.0+50*(y-1),Graphics::CAM_HEIGHT+50*(-z)));
        palm->setVisible(true);
    }
    std::array<std::array<std::tuple<float,float,float>, 4>, 5> new_fingers = hand.getFingers();
    for(int i = 0; i < 5; i++)
    {
       if(fingers[i][0])
       {
          float x,y,z;
          std::tie(x,y,z) = new_fingers[i][0];
          fingers[i][0]->setPosition(Graphics::convertLeapToIrr(x,y,z));
       }
       for(int j = 0; j < 3; j++)
       {
          /*irr::scene::IMesh* mesh = smgr->getGeometryCreator()->createCylinderMesh(
                                                                   1,
                                                                   1,
                                                                   10);*/
          //fingers[i][j*2+1] = smgr->addMeshSceneNode(mesh);
          //fingers[i][j*2+1]->setVisible(false);
          //fingers[i][j*2+1]->setPosition(irr::core::vector3df(-1,-1,-1));
          float x,y,z;
          std::tie(x,y,z) = new_fingers[i][j];
          fingers[i][j*2+2]->setPosition(Graphics::convertLeapToIrr(x,y,z));
	  fingers[i][j*2+2]->setVisible(true);
       }
    }

}
