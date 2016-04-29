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

    static const int CELL_WIDTH = 10;
    static const int CELL_HEIGHT = CELL_WIDTH;
    static const int OBJ_WIDTH = CELL_WIDTH * 0.75;
    static const int GRID_WIDTH = 5;
    static const int GRID_HEIGHT = GRID_WIDTH;
    static const int GRID_DEPTH = GRID_HEIGHT;
    static const int VIEW_WIDTH = CELL_WIDTH * (GRID_WIDTH + 1);
    static const int VIEW_HEIGHT = CELL_WIDTH * (GRID_HEIGHT + 1);
    static const int VIEW_DEPTH = CELL_WIDTH * (GRID_DEPTH + 1);
    static const int CAM_HEIGHT = CELL_WIDTH * GRID_WIDTH;
    static const int max_text_length;

    static irr::core::vector3df convertLeapToIrr(float,float,float);

    static Graphics* getInstance();

    void newObjects( std::vector<DirObject> );
    std::wstring pickUpHighlighted();
    void dropHeld(bool dropInDir);
    
    static void initGraphics();
    static void waitForInit();
    static void killGraphics();

    std::wstring currentHighlightedPath();
    static const gridcoord INVALID_POSITION;

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
    //std::vector<scene::ISceneNode*> dirNodes;
    std::array<scene::ICameraSceneNode*, NUM_CAMS> cams;
    gridcoord currentHighlightPosition;

    void createCameras();
    void emptyNodes();
    void fillNodes();
    void checkScroll(scene::ICameraSceneNode*, EventListener&);
    void checkTilt(scene::ICameraSceneNode* cam, EventListener& receiver);
    void drawHands();
    void setLayerTranslucency(bool b, int z);
    void setLayerTranslucencies(int z);
    gridcoord convertToLDS(float x, float y, float z);
    gridcoord convertToLDS(std::tuple<float,float,float>);
    void addObjAt(std::vector<DirObject>, gridcoord);
    //std::pair<gridcoord, DirObject> 

};

#endif
