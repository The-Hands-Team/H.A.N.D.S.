#include "Graphics.hpp"
#include "GestureCapture/GestureCapture.hpp"
#include <irrlicht/irrlicht.h>

using namespace irr;

    #ifdef _IRR_WINDOWS_
    #pragma comment(lib, "Irrlicht.lib")
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
    #endif



/* Static Members */
Graphics* Graphics::instance = nullptr;
std::promise<bool> Graphics::isGraphicsReady{};

/*const int Graphics::width = 10;
const int Graphics::unit_size = 5;
const int Graphics::view_height = width*10;*/

const gridcoord Graphics::INVALID_POSITION = gridcoord(-1,-1,-1);


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
    , receiver()
    , leftHand()
    , rightHand()
    , run (true)
    , need_node_update(false)
    , tiltingR(false)
    , tiltingL(false)
    , tiltingU(false)
    , tiltingD(false)
    , currentHighlightPosition(INVALID_POSITION)
{
    for( size_t i = 0; nullptr == device && i < preferedDrivers.size(); i++ )
    {
        device = createDevice(preferedDrivers[i],
                core::dimension2d<u32>(1920, 1200), 16, false, false, false, &receiver);
    }

    if( nullptr != device )
    {
        device->setResizable(false);
        driver = device->getVideoDriver();
        smgr = device->getSceneManager();
        env = device->getGUIEnvironment();

        createCameras();

        leftHand.init(smgr);
        rightHand.init(smgr);
    }
    else
    {
        run = false;
    }

    instance = this;

}

Graphics::~Graphics()
{
    objLock.lock();
    {
        dirObjects.clear();

        if( nullptr != device )
            device->drop();

        instance = nullptr;
    }
    objLock.unlock();
}


Graphics* Graphics::getInstance()
{
    return instance;
}

void Graphics::newObjects( std::vector<DirObject> objs )
{
    objLock.lock();
    if( instance )
    {
        dirObjects.clear();
        bool vecEmpty = objs.empty();
        for (int z = 0; z < GRID_DEPTH && !vecEmpty; z++)
        {
            for (int y = 0; y < GRID_HEIGHT && !vecEmpty; y++)
            {
                for (int x = 0; x < GRID_WIDTH && !vecEmpty; x++)
                {
                    if (objs.empty())
                    {
                        vecEmpty = true;
                    }
                    else
                    {
                        dirObjects.insert(std::make_pair<gridcoord, DirObject>(std::make_tuple(x,y,z), std::move(objs.back())));
                        objs.pop_back();
                    }
                }
            }
        }

        need_node_update.store(true);
    }
    objLock.unlock();
}

void Graphics::emptyNodes()
{
    /*for( scene::ISceneNode* node : dirNodes )
    {
        node->remove();
    }
    dirNodes.clear();*/
}

void Graphics::fillNodes()
{
    if( need_node_update.exchange(false) )
    {
        //emptyNodes();
        //for(std::pair<gridcoord, DirObject&> entry : dirObjects)
        for(auto entry = dirObjects.begin(); entry != dirObjects.end(); entry++)
        {
            DirObject& dirObj = entry->second;
            float Xpos,Ypos,Zpos;
            std::tie(Xpos,Ypos,Zpos) = entry->first;
            scene::IMeshSceneNode* newNode;
            scene::IBillboardTextSceneNode* newTextNode;
            if(dirObj.getType() == 'f')
            {
                newNode = smgr->addSphereSceneNode(0.5*OBJ_WIDTH);
            }
            else
            {
                newNode = smgr->addCubeSceneNode(OBJ_WIDTH);
            }

            if(newNode)
            {
                //dirNodes.push_back(newNode);
                dirObj.setNode(newNode);

                newNode->setPosition(core::vector3df
                        (
                         Xpos * CELL_WIDTH + CELL_WIDTH*1.5,
                         -(Ypos * CELL_WIDTH + CELL_WIDTH/2.0),
                         Zpos * CELL_HEIGHT + CAM_HEIGHT
                        ));
		/*std::cout << "Given pos: " << Xpos << "," << Ypos << "," << Zpos << std::endl;
                float fa,fb,fc;
                fa = newNode->getPosition().X;
                fb = newNode->getPosition().Y;
                fc = newNode->getPosition().Z;
		int a,b,c;
                std::tie(a,b,c) = convertToLDS(fa,fb,fc);
                std::cout << "Calc pos: " << a << ", " << b << ", " << c << std::endl;*/
                std::wstring finished_name( dirObj.getName() );

                if( !dirObj.isHighlighted && finished_name.length() > max_text_length )
                {
                    finished_name.erase( max_text_length - 3 );
                    finished_name += L"...";
                }
                smgr->getMeshManipulator()->setVertexColors(newNode->getMesh(), irr::video::SColor(255,0,0,255));
                if(dirObj.isSelected)
                {
                    newNode->setMaterialFlag(video::EMF_WIREFRAME, true);
                }
                newNode->setMaterialFlag(video::EMF_LIGHTING, true);
                newTextNode = smgr->addBillboardTextSceneNode
                    (
                     env->getFont("media/bigfont.png"),
                     finished_name.c_str(),
                     newNode,
                     core::dimension2d<f32>( finished_name.length() * (CELL_WIDTH/(float) max_text_length), CELL_WIDTH*0.2),
                     core::vector3df(0,0,-OBJ_WIDTH/2.0 - CELL_WIDTH/10.0),
                     -1,
                     //dirNodes.size(),
                     video::SColor(100,255,255,255),
                     video::SColor(100,255,255,255)
                    );
                dirObj.setNodeText(newTextNode);

            }
        }

    }
}

void Graphics::drawHands()
{
    bool leftHandFound = false;
    bool rightHandFound = false;
    std::vector<Hand> hands = GestureCapture::getInstance()->getHands();

    //std::cout << hands.size() << std::endl;

    for(size_t i = 0; i < hands.size() && i < 2; i++)
    {
        if(  hands[i].isRight())
        {
            rightHand.copyHand(hands[i]);
            rightHandFound = true;
            float x,y,z;
            std::tie(x,y,z) = hands[i].getPalmLocation();
            //std::cout << "Right: " << x << ',' << y << ',' << z << std::endl;
        }
        else
        {
            leftHand.copyHand(hands[i]);
            leftHandFound = true;
            float x,y,z;
            std::tie(x,y,z) = hands[i].getPalmLocation();
            //std::cout << "Left : " << x << ',' << y << ',' << z << std::endl;
        }
    }
    
    if( INVALID_POSITION != currentHighlightPosition
            && dirObjects.count(currentHighlightPosition) )
        {
            // TODO remove highlight from following
            dirObjects.at(currentHighlightPosition).setIsHighlighted(false, driver);
        }
    
    if( !leftHandFound )
    {
        leftHand.setVisible(false);
    }
    if( !rightHandFound )
    {
        rightHand.setVisible(false);
        setLayerTranslucencies(-1);
    }
    else
    {
        if( INVALID_POSITION != currentHighlightPosition
            && dirObjects.count(currentHighlightPosition) )
        {
            // TODO remove highlight from following
            dirObjects.at(currentHighlightPosition).setIsHighlighted(false, driver);
        }
        gridcoord pos = convertToLDS(rightHand.getXYZ());
        if( dirObjects.count(pos) )
        {
            // TODO add highlight to following
            dirObjects.at(pos).setIsHighlighted(true, driver);

        }
        currentHighlightPosition = pos;
        setLayerTranslucencies(std::get<2>(pos) + 0.5);
    }
}

void Graphics::setLayerTranslucency(bool b, int z)
{
    for(int i = 0; i < GRID_WIDTH; i++)
    {
        for(int j = 0; j < GRID_HEIGHT; j++)
        {
            gridcoord coord = gridcoord(i,j,z);
            if(dirObjects.count(coord))
            {
               dirObjects.at(coord).setTranslucent(b);
            }
        }
    }
}

void Graphics::setLayerTranslucencies(int z)
{
    for(int i = 0; i < GRID_DEPTH; i++)
    {
       setLayerTranslucency(i<z,i);
    }
}

gridcoord Graphics::convertToLDS(float x, float y, float z)
{
    return std::make_tuple((x - CELL_WIDTH*1.5)/CELL_WIDTH + 0.5,
                           (-y - CELL_WIDTH/2.0)/CELL_WIDTH + 0.5,
                           (z - CAM_HEIGHT)/CELL_HEIGHT + 0.5);
}

gridcoord Graphics::convertToLDS(std::tuple<float,float,float> coords)
{
    return convertToLDS(std::get<0>(coords), std::get<1>(coords), std::get<2>(coords));
}

irr::core::vector3df Graphics::convertLeapToIrr(float x, float y, float z)
{
   return irr::core::vector3df(Graphics::VIEW_WIDTH/2.0+50*x,-Graphics::VIEW_HEIGHT/2.0+2*CELL_WIDTH+50*(y-1),Graphics::CAM_HEIGHT+CELL_WIDTH+50*(-z));
}

void Graphics::createCameras()
{
    //create persp camera
    float mid = VIEW_WIDTH / 2.0;
    cams[CAM_PERSP] = smgr->addCameraSceneNode(
                        0,
                        core::vector3df(mid,-mid,-CAM_HEIGHT),//-2 * CAM_HEIGHT),
                        core::vector3df(mid,-mid,0),
                        -1,
                        true);
    cams[CAM_PERSP]->setFOV( ( VIEW_HEIGHT/CAM_HEIGHT ) * 3.1415 / 5.0f);// / 180.0f );

    //create orth camera
    cams[CAM_ORTH] = smgr->addCameraSceneNode(
                            0,
                            core::vector3df(mid,-mid,-CAM_HEIGHT),
                            core::vector3df(mid,-mid,0),
                            -1,
                            true);
    core::matrix4 projMat;
    projMat.buildProjectionMatrixOrthoLH(VIEW_WIDTH,VIEW_HEIGHT,0,CAM_HEIGHT+VIEW_DEPTH);
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
    float mid = VIEW_WIDTH / 2.0;
    smgr->addLightSceneNode(nullptr,
                            core::vector3df(0,0,-CAM_HEIGHT),
                            video::SColorf(1.0f,1.0f,1.0f),
                            100.0f,
                            -1
                           );

    smgr->setActiveCamera(cams[CAM_PERSP]);

    while( run && device->run() )
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
        checkScroll(cam, receiver);
        checkTilt(cam, receiver);


        objLock.lock();
        { 
            fillNodes();
            drawHands();

            driver->beginScene(true, true, video::SColor(255,113,113,133));

            smgr->drawAll(); // draw the 3d scene

            driver->endScene();
        }
        objLock.unlock();

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
            run = false;
        }
    }
    objLock.lock();
    {
        instance = nullptr;
        dirObjects.clear();
        while(device->run())
        {
            device->closeDevice();
        }
    }
    objLock.unlock();
}

void Graphics::waitForInit()
{
    isGraphicsReady.get_future().wait();
}


//Because Irrlicht is either too dumb or too smart for its own good
void Graphics::initGraphics()
{
    Graphics g{};
    isGraphicsReady.set_value(true);
    if( g.run )
        g.mainLoop();
}
void Graphics::killGraphics()
{
    if( instance )
    {
    instance->run = false;

    }
}

std::wstring Graphics::currentHighlightedPath()
{
    if (dirObjects.count(currentHighlightPosition))
    {
        return dirObjects.at(currentHighlightPosition).getName();
    }
    else
    {
        return std::wstring();
    }
}






void Graphics::checkScroll(irr::scene::ICameraSceneNode* cam, EventListener& receiver)
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

void Graphics::checkTilt(scene::ICameraSceneNode* cam, EventListener& receiver)
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
