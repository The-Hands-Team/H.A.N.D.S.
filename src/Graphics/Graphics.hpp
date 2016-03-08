#include "DirObject.hpp"
#include <vector>
#include <array>
#include <mutex>
#include "EventListener.hpp"
#include "irrlicht/ISceneNode.h"
#include "irrlicht/ICameraSceneNode.h"
#include "irrlicht/IVideoDriver.h"
#include "irrlicht/ISceneManager.h"
#include "irrlicht/IGUIEnvironment.h"
#include "irrlicht/IrrlichtDevice.h"

#ifndef GRAPHICS_H
#define GRAPHICS_H

using namespace irr;

class Graphics
{

public:
    Graphics();
    ~Graphics();
    void mainLoop();
    static Graphics* getInstance();

    void newObjects( const std::vector<DirObject>& );

    static void initGraphics();
    static void killGraphics();


private:

    enum
    {
        CAM_PERSP = 0,
        CAM_ORTH,
        NUM_CAMS
    };

    static Graphics* instance;
    static const int width;
    static const int unit_size;
    static const int view_height;
    static const int max_text_length;
    static const std::array<video::E_DRIVER_TYPE, 6> preferedDrivers;

    IrrlichtDevice*       device;
    video::IVideoDriver*  driver;
    scene::ISceneManager* smgr;
    gui::IGUIEnvironment* env;
    EventListener         receiver;

    bool run;

    bool tiltingR;
    bool tiltingL;
    bool tiltingU;
    bool tiltingD;

    std::mutex objLock;
    std::vector<DirObject> dirObjects;
    std::vector<scene::ISceneNode*> dirNodes;
    std::array<scene::ICameraSceneNode*, NUM_CAMS> cams;

    void createCameras();
    void emptyNodes();
    void fillNodes();
    void checkScroll(scene::ICameraSceneNode*, EventListener&);
    void checkTilt(scene::ICameraSceneNode* cam, EventListener& receiver);

};

#endif
