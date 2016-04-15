#include "DirObject.hpp"
#include <vector>
#include <array>
#include <atomic>
#include <mutex>
#include <future>
#include <unordered_map>
#include <tuple>
#include "EventListener.hpp"
#include "irrlicht/ISceneNode.h"
#include "irrlicht/ICameraSceneNode.h"
#include "irrlicht/IVideoDriver.h"
#include "irrlicht/ISceneManager.h"
#include "irrlicht/IGUIEnvironment.h"
#include "irrlicht/IrrlichtDevice.h"
#include "GHand.hpp"

#ifndef GRAPHICS_H
#define GRAPHICS_H

using namespace irr;

using gridcoord = std::tuple<int,int,int>;

namespace std{
    template <>
    struct hash<std::tuple<int,int,int>>
    {
        size_t operator()(const std::tuple<int,int,int>& key) const
        {
            int x,y,z;
            tie(x,y,z) = key;
            return z*100 + y*10 + x;
        }
    };
}

class Graphics
{

public:
    static Graphics* getInstance();

    void newObjects( std::vector<DirObject> );
    
    static void initGraphics();
    static void waitForInit();
    static void killGraphics();


private:

    enum
    {
        CAM_PERSP = 0,
        CAM_ORTH,
        NUM_CAMS
    };
    
    Graphics();
    ~Graphics();
    void mainLoop();

    static Graphics* instance;
    static std::promise<bool> isGraphicsReady;
    
    static const int VIEW_WIDTH;
    static const int VIEW_HEIGHT;
    static const int VIEW_DEPTH;
    static const int CELL_WIDTH;
    static const int OBJ_WIDTH;
    static const int GRID_WIDTH;
    static const int GRID_HEIGHT;
    static const int GRID_DEPTH;
    static const int CAM_HEIGHT;
    static const gridcoord INVALID_POSITION;

    static const int max_text_length;
    static const std::array<video::E_DRIVER_TYPE, 6> preferedDrivers;

    IrrlichtDevice*       device;
    video::IVideoDriver*  driver;
    scene::ISceneManager* smgr;
    gui::IGUIEnvironment* env;
    EventListener         receiver;
    GHand leftHand;
    GHand rightHand;

    bool run;

    std::atomic_bool need_node_update;

    bool tiltingR;
    bool tiltingL;
    bool tiltingU;
    bool tiltingD;

    std::mutex objLock;
    std::unordered_map<gridcoord,DirObject> dirObjects;
    std::vector<scene::ISceneNode*> dirNodes;
    std::array<scene::ICameraSceneNode*, NUM_CAMS> cams;
    gridcoord currentHighlightPosition;

    void createCameras();
    void emptyNodes();
    void fillNodes();
    void checkScroll(scene::ICameraSceneNode*, EventListener&);
    void checkTilt(scene::ICameraSceneNode* cam, EventListener& receiver);
    void drawHands();
    gridcoord convertToLDS(float x, float y, float z);
    gridcoord convertToLDS(std::tuple<float,float,float>);
    //std::pair<gridcoord, DirObject> 

};

#endif
