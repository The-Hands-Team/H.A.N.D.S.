#include "GHand.hpp"
#include "Graphics.hpp"
#include "irrlicht/irrlicht.h"

#include <tuple>

GHand::GHand()
: palm(nullptr)
, fingers({{nullptr}})
{}

void GHand::init(irr::scene::ISceneManager* smgr)
{
    irr::video::SColor handColor(255, 200,200,200);
    palm = smgr->addSphereSceneNode(Graphics::CELL_WIDTH/3.0);
    smgr->getMeshManipulator()->setVertexColors(palm->getMesh(), handColor);
    palm->setMaterialFlag(video::EMF_LIGHTING, false);
    palm->setVisible(false);
    palm->setPosition(irr::core::vector3df(-1,-1,-1));

    selector = smgr->addSphereSceneNode(Graphics::CELL_WIDTH/9.0);
    smgr->getMeshManipulator()->setVertexColors(selector->getMesh(), irr::video::SColor(0,0,255,127));
    selector->setMaterialFlag(video::EMF_LIGHTING, false);
    selector->setVisible(false);
    selector->setPosition(irr::core::vector3df(-1,-1,-1));

    std::cout << "Calling init!\n";
    std::cout << selector->getMaterialCount() << std::endl;

    for(int i = 0; i < 5; i++)
    {
       fingers[i][0] = smgr->addSphereSceneNode(Graphics::CELL_WIDTH/6.0);
       fingers[i][0]->setVisible(false);
       smgr->getMeshManipulator()->setVertexColors(fingers[i][0]->getMesh(), handColor);
       fingers[i][0]->setMaterialFlag(video::EMF_LIGHTING, false);
       for(int j = 0; j < 3; j++)
       {
          /*irr::scene::IMesh* mesh = smgr->getGeometryCreator()->createCylinderMesh(
                                                                   1,
                                                                   1,
                                                                   10);
          fingers[i][j*2+1] = smgr->addMeshSceneNode(mesh);
          fingers[i][j*2+1]->setVisible(false);
          fingers[i][j*2+1]->setPosition(irr::core::vector3df(-1,-1,-1));*/
          fingers[i][j*2+2] = smgr->addSphereSceneNode(Graphics::CELL_WIDTH/6.0);
            smgr->getMeshManipulator()->setVertexColors(fingers[i][j*2+2]->getMesh(), handColor);
            fingers[i][j*2+2]->setMaterialFlag(video::EMF_LIGHTING, false);
          fingers[i][j*2+2]->setVisible(false);
          fingers[i][j*2+2]->setPosition(irr::core::vector3df(-1,-1,-1));
       }
    }


setXYZ(50,50,50);
}

void GHand::deinit()
{
    palm->remove();
    palm = nullptr;

    for(auto finger : fingers)
        for(irr::scene::ISceneNode* node : finger)
            if( nullptr != node )
                node->remove();
}

GHand::~GHand()
{
}

void GHand::setXYZ(irr::f32 x,irr::f32 y, irr::f32 z)
{
    if(palm)
        palm->setPosition(irr::core::vector3df(x,y,z));
}
std::tuple<float,float,float> GHand::getXYZ()
{
    irr::core::vector3df pos(-1,-1,-1);

    if(selector)//palm)
    {
        pos = selector->getPosition();//palm->getPosition();
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
    if(selector)
    {
        selector->setVisible(vis);
        if( false == vis )
        {
            selector->setPosition(irr::core::vector3df(-1,-1,-1));
        }
    }
    for(int i = 0; i < 5; i++)
    {
        if (fingers[i][0])
        {
            fingers[i][0]->setVisible(vis);
            if ( false == vis )
            {
                fingers[i][0]->setPosition(irr::core::vector3df(-1,-1,-1));
            }
        }
        for(int j = 0; j < 3; j++)
        {
            if (fingers[i][j*2+2])
            {
                fingers[i][j*2+2]->setVisible(vis);
                if ( false == vis )
                {
                    fingers[i][j*2+2]->setPosition(irr::core::vector3df(-1,-1,-1));
                }
            }
        }
    }
}

void GHand::copyHand(Hand& hand)
{
    if(palm)
    {
        float x,y,z;
        std::tie(x,y,z) = hand.getPalmLocation();
        //palm->setPosition(irr::core::vector3df(x*100+100,z*100+100,(1.5-y)*100-100));
        //palm->setPosition(irr::core::vector3df(Graphics::VIEW_WIDTH/2.0+50*x,-Graphics::VIEW_HEIGHT/2.0+50*(y-1),Graphics::CAM_HEIGHT+50*(-z)));
        palm->setPosition(irr::core::vector3df(Graphics::convertLeapToIrr(x,y,z)));
        palm->setVisible(true);
    }
    std::array<std::array<std::tuple<float,float,float>, 4>, 5> new_fingers = hand.getFingers();
    if(selector)
    {
        float x1,y1,z1;
        float x2,y2,z2;
        std::tie(x1,y1,z1) = new_fingers[0][3];
        std::tie(x2,y2,z2) = new_fingers[1][3];
        selector->setPosition(irr::core::vector3df(Graphics::convertLeapToIrr((x1+x2)/2.0,(y1+y2)/2.0,(z1+z2)/2.0)));
        selector->setVisible(true);
    }
    for(int i = 0; i < 5; i++)
    {
       if(fingers[i][0])
       {
          float x,y,z;
          std::tie(x,y,z) = new_fingers[i][0];
          fingers[i][0]->setPosition(Graphics::convertLeapToIrr(x,y,z));
          fingers[i][0]->setVisible(true);
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
          std::tie(x,y,z) = new_fingers[i][j+1];
          fingers[i][j*2+2]->setPosition(Graphics::convertLeapToIrr(x,y,z));
          //std::cout << "i,j: " << i << "," << j << "; x,y,z: " << x << "," << y << "," << z << std::endl;
	  fingers[i][j*2+2]->setVisible(true);
       }
    }

}
