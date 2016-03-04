#include "Graphics.hpp"
#include <irrlicht/irrlicht.h>

using namespace irr;
using namespace std;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

    #ifdef _IRR_WINDOWS_
    #pragma comment(lib, "Irrlicht.lib")
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
    #endif



/* Static Members */
Graphics* Graphics::instance = nullptr;

const int Graphics::width = 10;
const int Graphics::unit_size = 5;
const int Graphics::view_height = width*10;
const int Graphics::max_text_length = 11;

const std::array<video::E_DRIVER_TYPE, 6> Graphics::preferedDrivers = { video::EDT_DIRECT3D9
                                                                      , video::EDT_DIRECT3D8
                                                                      , video::EDT_OPENGL
                                                                      , video::EDT_BURNINGSVIDEO
                                                                      , video::EDT_SOFTWARE
                                                                      , video::EDT_NULL };

/*  Functions  */

Graphics::Graphics()
    : device(nullptr)
    , driver(nullptr)
    , smgr(nullptr)
    , env(nullptr)
    , tiltingR(false)
    , tiltingL(false)
    , tiltingU(false)
    , tiltingD(false)
{

    instance = this;

    for( size_t i = 0; nullptr == device && i < preferedDrivers.size(); i++ )
    {
        device = createDevice(preferedDrivers[i],
                core::dimension2d<u32>(1366/2, 768), 16, false, false, false, &receiver);
    }

    if( nullptr == device )
    {
        //TODO handle gracefully
        exit(1);
    }

    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    env = device->getGUIEnvironment();

    createCameras();

    smgr->addBillboardTextSceneNode
            (
                env->getFont("media/bigfont.png"),
                L"Testing Things",
                0,
                core::dimension2d<f32>( 100 * 0.75, 3.0f),
                core::vector3df(0,0,-7),
                8888888,
                video::SColor(100,255,255,255),
                video::SColor(100,255,255,255)
            );
}

Graphics::~Graphics()
{
    if( nullptr != device )
        device->drop();

    instance = nullptr;
}


Graphics* Graphics::getInstance()
{
    return instance;
}

void Graphics::newObjects( const std::vector<DirObject>& objs )
{
    objLock.lock();
    dirObjects.clear();
    dirObjects = objs;
    objLock.unlock();
}

void Graphics::emptyNodes()
{
    for( scene::ISceneNode* node : dirNodes )
    {
        node->remove();
    }
    dirNodes.clear();
}


void Graphics::fillNodes()
{
    objLock.lock();
    emptyNodes();
    for(DirObject dirObj : dirObjects)
    {
        scene::ISceneNode* newNode;
        if(dirObj.getType() == 'f')
        {
            newNode = smgr->addSphereSceneNode();
        }
        else
        {
            newNode = smgr->addCubeSceneNode();
        }

        //std::wcout << dirObj.getName() << std::endl;

        if(newNode)
        {
            dirNodes.push_back(newNode);

            newNode->setPosition(core::vector3df
                (
                    dirObj.getX() * unit_size*width,
                    -dirObj.getY() * unit_size*width,
                    50
                ));

            std::wstring finished_name( *(dirObj.getName()) );

            if( !dirObj.isHighlighted && finished_name.length() > max_text_length )
            {
                finished_name.erase( max_text_length - 3 );
                finished_name += L"...";
            }

            if(dirObj.isHighlighted)
            {
                newNode->setMaterialTexture(0, driver->getTexture("media/selected.jpg"));
            }
            else
            {
                newNode->setMaterialTexture(0, driver->getTexture("media/unselected.jpg"));
            }
            if(dirObj.isSelected)
            {
                newNode->setMaterialFlag(video::EMF_WIREFRAME, true);
            }
            newNode->setMaterialFlag(video::EMF_LIGHTING, false);
            /*smgr->addBillboardTextSceneNode
            (
                env->getFont("media/bigfont.png"),
                finished_name.c_str(),
                newNode,
                core::dimension2d<f32>( finished_name.length() * 0.75, 3.0f),
                core::vector3df(0,0,-7),
                dirNodes.size(),
                video::SColor(100,255,255,255),
                video::SColor(100,255,255,255)
            );*/
            irr::video::ITexture* rt = driver->addRenderTargetTexture(core::dimension2d<u32>(512,512), "rt", ECF_R8G8B8);
            
            driver->setRenderTarget(rt, true, true, video::SColor(255,255,255,255));
            driver->draw2DImage(driver->getTexture("media/unselected.jpg"), position2d<s32>(0, 0));
            env->getBuiltInFont()->draw(finished_name.c_str(), rect<s32>(0,0,unit_size,unit_size), SColor(255, 0, 0, 255), true, true, 0);
            driver->setRenderTarget(0);
            
            newNode->setMaterialTexture(0, rt);

        }
    }
    objLock.unlock();
}

void Graphics::checkScroll(irr::scene::ICameraSceneNode* cam, EventListener receiver)
{
   if(receiver.IsKeyDown(irr::KEY_KEY_Z))
   {
  cam->setPosition(core::vector3df(cam->getPosition().X, cam->getPosition().Y+1, cam->getPosition().Z));
  cam->setTarget(core::vector3df(cam->getTarget().X, cam->getTarget().Y+1, cam->getTarget().Z));
   }
   if(receiver.IsKeyDown(irr::KEY_KEY_X))
   {
  cam->setPosition(core::vector3df(cam->getPosition().X, cam->getPosition().Y-1, cam->getPosition().Z));
  cam->setTarget(core::vector3df(cam->getTarget().X, cam->getTarget().Y-1, cam->getTarget().Z));
   }
}

void Graphics::checkTilt(scene::ICameraSceneNode* cam, EventListener receiver)
{
   if(receiver.IsKeyDown(irr::KEY_KEY_I) && !tiltingU)
   {
      cam->setPosition(core::vector3df(cam->getPosition().X, cam->getPosition().Y+25, cam->getPosition().Z));
      tiltingU = true;
   }
   else if(!receiver.IsKeyDown(irr::KEY_KEY_I) && tiltingU)
   {
      cam->setPosition(core::vector3df(cam->getPosition().X, cam->getPosition().Y-25, cam->getPosition().Z));
  tiltingU = false;
   }

   if(receiver.IsKeyDown(irr::KEY_KEY_K) && !tiltingD)
   {
      cam->setPosition(core::vector3df(cam->getPosition().X, cam->getPosition().Y-25, cam->getPosition().Z));
      tiltingD = true;
   }
   else if(!receiver.IsKeyDown(irr::KEY_KEY_K) && tiltingD)
   {
      cam->setPosition(core::vector3df(cam->getPosition().X, cam->getPosition().Y+25, cam->getPosition().Z));
  tiltingD = false;
   }

   if(receiver.IsKeyDown(irr::KEY_KEY_J) && !tiltingL)
   {
      cam->setPosition(core::vector3df(cam->getPosition().X-25, cam->getPosition().Y, cam->getPosition().Z));
      tiltingL = true;
   }
   else if(!receiver.IsKeyDown(irr::KEY_KEY_J) && tiltingL)
   {
      cam->setPosition(core::vector3df(cam->getPosition().X+25, cam->getPosition().Y, cam->getPosition().Z));
  tiltingL = false;
   }

   if(receiver.IsKeyDown(irr::KEY_KEY_L) && !tiltingR)
   {
      cam->setPosition(core::vector3df(cam->getPosition().X+25, cam->getPosition().Y, cam->getPosition().Z));
      tiltingR = true;
   }
   else if(!receiver.IsKeyDown(irr::KEY_KEY_L) && tiltingR)
   {
      cam->setPosition(core::vector3df(cam->getPosition().X-25, cam->getPosition().Y, cam->getPosition().Z));
  tiltingR = false;
   }
}

void Graphics::createCameras()
{
    //create persp camera
    float mid = (width * unit_size) / 2.0;
    cams[CAM_PERSP] = smgr->addCameraSceneNode(
                        0,
                        core::vector3df(mid,-mid,0),
                        core::vector3df(mid,-mid,view_height),
                        -1,
                        true);

    //create orth camera
    cams[CAM_ORTH] = smgr->addCameraSceneNode(
                            0,
                            core::vector3df(mid,-mid,0),
                            core::vector3df(mid,-mid,view_height),
                            -1,
                            true);
    core::matrix4 projMat;
    projMat.buildProjectionMatrixOrthoLH(75,75,10,100);
    cams[CAM_ORTH]->setProjectionMatrix(projMat, true);
}

void Graphics::mainLoop()
{
    /*
    We have done everything, so lets draw it. We also write the current
    frames per second and the name of the driver to the caption of the
    window.
    */
    int lastFPS = -1;

    // In order to do framerate independent movement, we have to know
    // how long it was since the last frame
    // u32 then = device->getTimer()->getTime();

    while( device->run() )
    {

        if(receiver.IsKeyDown(irr::KEY_KEY_P))
        {
            smgr->setActiveCamera(cams[CAM_PERSP]);
        }
        if(receiver.IsKeyDown(irr::KEY_KEY_O))
        {
            smgr->setActiveCamera(cams[CAM_ORTH]);
        }

        irr::scene::ICameraSceneNode* cam = smgr->getActiveCamera();
        fillNodes();

        checkScroll(cam, receiver);
        checkTilt(cam, receiver);


        driver->beginScene(true, true, video::SColor(255,113,113,133));

        smgr->drawAll(); // draw the 3d scene

        driver->endScene();

        int fps = driver->getFPS();

        if (lastFPS != fps)
        {
            core::stringw tmp(L"Tests are things that are important");
            tmp += driver->getName();
            tmp += L"] fps: ";
            tmp += fps;

            device->setWindowCaption(tmp.c_str());
            lastFPS = fps;
        }

        if(receiver.IsKeyDown(irr::KEY_ESCAPE))
        {
            device->closeDevice();
        }
    }
}
