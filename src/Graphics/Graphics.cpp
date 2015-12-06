#include <irrlicht/irrlicht.h>
#include "irrlicht/driverChoice.h"
#include "Graphics.hpp"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

video::IVideoDriver* driver;
scene::ISceneManager* smgr;
gui::IGUIEnvironment* env;

class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	MyEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

struct dirObject
{
public:
	dirObject(char i_type, int i_x, int i_y, wchar_t* i_name)
	{
		type = i_type;
		x = i_x;
		y = i_y;
		name = i_name;
	}
	char getType()
	{
		return type;
	}
	int getX()
	{
		return x;
	}
	int getY()
	{
		return y;
	}
	wchar_t* getName()
	{
		return name;
	}
private:
	char type;
	int x;
	int y;
	wchar_t* name;
};

const int maxDirObjects = 100;
int numDirObjects = 0;
int numDirNodes = 0;
dirObject* dirObjects[maxDirObjects];
scene::ISceneNode* dirNodes[maxDirObjects];
scene::IBillboardTextSceneNode* dirBillboards[maxDirObjects];

void initObjects()
{
	numDirObjects = 10;
	dirObjects[0] = new dirObject('f', 0, 0, L"0,0");
	dirObjects[1] = new dirObject('f', 0, 1, L"0,1");
	dirObjects[2] = new dirObject('d', 0, 2, L"1,2");
	dirObjects[3] = new dirObject('d', 0, 3, L"1,3");
	dirObjects[4] = new dirObject('d', 0, 4, L"1,4");
	dirObjects[5] = new dirObject('d', 0, 5, L"1,5");
	dirObjects[6] = new dirObject('d', 0, 6, L"1,6");
	dirObjects[7] = new dirObject('d', 0, 7, L"1,7");
	dirObjects[8] = new dirObject('d', 0, 8, L"1,8");
	dirObjects[9] = new dirObject('d', 0, 9, L"1,9");
}

void emptyNodes()
{
	for(int i = 0; i < numDirNodes; i++)
	{
		if(dirNodes[i])
			dirNodes[i]->remove();
	}
}

void fillNodes()
{
	emptyNodes();
	for(int i = 0; i < numDirObjects; i++)
	{
		if(dirObjects[i]->getType() == 'f')
		{
			dirNodes[i] = smgr->addSphereSceneNode();
		}
		else
		{
			dirNodes[i] = smgr->addCubeSceneNode();
		}

		if(dirNodes[i])
		{
			dirNodes[i]->setPosition(core::vector3df
				(
					dirObjects[i]->getX()*10 + 10,
					dirObjects[i]->getY()*10+20,
					50
				));
			dirNodes[i]->setMaterialTexture(0, driver->getTexture("media/wall.bmp"));
			dirNodes[i]->setMaterialFlag(video::EMF_LIGHTING, false);
			dirBillboards[i] = smgr->addBillboardTextSceneNode
				(
					env->getFont("media/fontcourier.bmp"),
					dirObjects[i]->getName(),
					dirNodes[i],core::dimension2d<f32>(8.0f, 5.0f),
					core::vector3df(0,0,-5),
					i,
					SColor(100,255,0,0),
					SColor(100,0,0,255)
				);
		}

	}
	numDirNodes = numDirObjects;
}

/*
This is the main method. We can now use main() on every platform.
*/
void initGraphics()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return;

	// create device
	MyEventReceiver receiver;

	IrrlichtDevice* device = createDevice(driverType,
			core::dimension2d<u32>(640, 480), 16, false, false, false, &receiver);

	if (device == 0)
	    return; // could not create selected driver.

	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	env = device->getGUIEnvironment();

	/*
	To be able to look at and move around in this scene, we create a first
	person shooter style camera and make the mouse cursor invisible.
	smgr->addCameraSceneNodeFPS();
	device->getCursorControl()->setVisible(false);
	*/

	//create camera
	smgr->addCameraSceneNode(0,core::vector3df(50,50,0),core::vector3df(50,50,100),-1,true);

	/*
	We have done everything, so lets draw it. We also write the current
	frames per second and the name of the driver to the caption of the
	window.
	*/
	int lastFPS = -1;

	// In order to do framerate independent movement, we have to know
	// how long it was since the last frame
	u32 then = device->getTimer()->getTime();

	initObjects();

	while(device->run())
	{
		fillNodes();

		if(receiver.IsKeyDown(irr::KEY_ESCAPE))
		{
			exit(0);
		}
		if(receiver.IsKeyDown(irr::KEY_KEY_W))
		{

		}


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
	}

	/*
	In the end, delete the Irrlicht device.
	*/
	device->drop();
	
	exit(0);
}

/*
That's it. Compile and run.
**/
