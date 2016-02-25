#include "DirObject.hpp"
#include <vector>
#include <mutex>
#include "EventListener.hpp"
#include "irrlicht/ISceneNode.h"
#include "irrlicht/ICameraSceneNode.h"
#include "irrlicht/IVideoDriver.h"
#include "irrlicht/ISceneManager.h"
#include "irrlicht/IGUIEnvironment.h"

#ifndef GRAPHICS_H
#define GRAPHICS_H

using namespace irr;

class Graphics
{

public:

    Graphics();

    void newObjects( const std::vector<DirObject>& );

    static Graphics* getInstance();


private:
    static Graphics* instance;

    video::IVideoDriver* driver;
    scene::ISceneManager* smgr;
    gui::IGUIEnvironment* env;

    static const int width;
    static const int unit_size;
    static const int view_height;
    static const int max_text_length;

    std::mutex objLock;
    std::vector<DirObject> dirObjects;
    std::vector<scene::ISceneNode*> dirNodes;

    void emptyNodes();
    void fillNodes();
    void checkScroll(scene::ICameraSceneNode*, EventListener);

};

#endif
