/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>
#include <cstring>
#include <thread>
#include "Leap.h"
#include "GestureEvent.hpp"
#include "../MainController/MainController.hpp"

using namespace Leap;

class GestureCapture : public Listener {
    public:

        virtual void onInit(const Controller&);
        virtual void onConnect(const Controller&);
        virtual void onDisconnect(const Controller&);
        virtual void onExit(const Controller&);
        virtual void onFrame(const Controller&);
        virtual void onFocusGained(const Controller&);
        virtual void onFocusLost(const Controller&);
        virtual void onDeviceChange(const Controller&);
        virtual void onServiceConnect(const Controller&);
        virtual void onServiceDisconnect(const Controller&);

    private:
        bool activeGestures[INVALID_GESTURE]= { 0 };

};

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};
const std::string gestureNames[] = {"Circle", "Key Tap", "Screen Tap", "Swipe Up", "Swipe Down", "Swipe Right", "Swipe Left"};

void GestureCapture::onInit(const Controller& controller) {
  std::cout << "Leap Motion Initialized" << std::endl;
}

void GestureCapture::onConnect(const Controller& controller) {
  std::cout << "Leap Motion Connected" << std::endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void GestureCapture::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  std::cout << "Leap Motion Disconnected" << std::endl;
}

void GestureCapture::onExit(const Controller& controller) {
  std::cout << "Leap Motion Exited" << std::endl;
}

void GestureCapture::onFrame(const Controller& controller) {
    // Get the most recent frame and report some basic information
    const Frame frame = controller.frame();
    const Frame prevFrame = controller.frame(1);
    //Initialize all gestures to false
    bool curGestures[INVALID_GESTURE] = { 0 };

    // TODO don't ignore when the same gesture is made simultaneously by different fingers

    Leap::GestureList gestures = frame.gestures();
    for(Leap::GestureList::const_iterator gl = gestures.begin(); gl != gestures.end(); gl++)
    {
        switch ((*gl).type()) {
            case Leap::Gesture::TYPE_CIRCLE:
                curGestures[CIRCLE] = true;
                if(!activeGestures[CIRCLE])
                    MainController::getInstance()->pushEvent(new GestureEvent(gestureNames[CIRCLE], CIRCLE));
                activeGestures[CIRCLE] = true;
                break;
            case Leap::Gesture::TYPE_KEY_TAP:
                curGestures[KEY_TAP] = true;
                if(!activeGestures[KEY_TAP])
                    MainController::getInstance()->pushEvent(new GestureEvent(gestureNames[KEY_TAP], KEY_TAP));
                activeGestures[KEY_TAP] = true;
                break;
            case Leap::Gesture::TYPE_SCREEN_TAP:
                curGestures[SCREEN_TAP] = true;
                if(!activeGestures[SCREEN_TAP])
                    MainController::getInstance()->pushEvent(new GestureEvent(gestureNames[SCREEN_TAP], SCREEN_TAP));
                activeGestures[SCREEN_TAP] = true;
                break;
            case Leap::Gesture::TYPE_SWIPE:
            {
                    SwipeGesture sg = *gl;
                    Leap::Vector v = sg.direction();
                    GestureType swipeType = INVALID_GESTURE;
                    if( std::abs(v.y) > std::abs(v.x) )
                    {
                        if(v.y > 0)
                        {
                            swipeType = SWIPE_UP;
                        }
                        else
                        {
                            swipeType = SWIPE_DOWN;
                        }
                    }
                    else
                    {
                        if(v.x > 0)
                        {
                            swipeType = SWIPE_RIGHT;
                        }
                        else
                        {
                            swipeType = SWIPE_LEFT;
                        }
                    }
                    if(swipeType!=INVALID_GESTURE)
                    {
                        curGestures[swipeType] = true;
                        if(!activeGestures[swipeType])
                            MainController::getInstance()->pushEvent(new GestureEvent(gestureNames[swipeType] , swipeType));
                        activeGestures[swipeType] = true;
                    }
                    break;
                }
            default:
               std::cout<<"Leap Motion gesture not recognized\n";
               break;
        }
    }

    for(int i=0; i<INVALID_GESTURE; i++)
    {
        activeGestures[i] = curGestures[i];
    }
}

void GestureCapture::onFocusGained(const Controller& controller) {
  std::cout << "Leap Motion Focus Gained" << std::endl;
}

void GestureCapture::onFocusLost(const Controller& controller) {
  std::cout << "Leap Motion Focus Lost" << std::endl;
}

void GestureCapture::onDeviceChange(const Controller& controller) {
  std::cout << "Leap Motion Device Changed" << std::endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    std::cout << "id: " << devices[i].toString() << std::endl;
    std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
  }
}

void GestureCapture::onServiceConnect(const Controller& controller) {
  std::cout << "Leap Motion Service Connected" << std::endl;
}

void GestureCapture::onServiceDisconnect(const Controller& controller) {
  std::cout << "Leap Motion Service Disconnected" << std::endl;
}

void initGesture(bool background) {

  // Create a sample listener and controller
  GestureCapture listener;
  Controller controller;

  // Have the sample listener receive events from the controller
  controller.addListener(listener);

  if (background)
    controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

  // Keep this process running until Enter is pressed
  while(true);

  // Remove the sample listener when done
  controller.removeListener(listener);

}
