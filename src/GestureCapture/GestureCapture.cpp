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
#include "MainController/GestureQueue.hpp"

#include "GestureCapture.hpp"

using namespace Leap;

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};


void GestureCapture::onConnect(const Controller& controller) {
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
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
                    GestureQueue::getInstance()->push(new GestureMessage(CIRCLE, NONE));
                activeGestures[CIRCLE] = true;
                break;
            case Leap::Gesture::TYPE_SCREEN_TAP:
                curGestures[SCREEN_TAP] = true;
                if(!activeGestures[SCREEN_TAP])
                    GestureQueue::getInstance()->push(new GestureMessage(SCREEN_TAP, NONE));
                activeGestures[SCREEN_TAP] = true;
                break;
            case Leap::Gesture::TYPE_SWIPE:
            {
                    SwipeGesture sg = *gl;
                    Leap::Vector v = sg.direction();
                    Direction swipeType = NONE;
                    if( std::abs(v.y) > std::abs(v.x) )
                    {
                        if(v.y > 0)
                        {
                            swipeType = UP;
                        }
                        else
                        {
                            swipeType = DOWN;
                        }
                    }
                    else
                    {
                        if(v.x > 0)
                        {
                            swipeType = RIGHT;
                        }
                        else
                        {
                            swipeType = LEFT;
                        }
                    }
                    if(swipeType!=NONE)
                    {
                        curGestures[swipeType] = true;
                        if(!activeGestures[swipeType])
                            GestureQueue::getInstance()->push(new GestureMessage(SWIPE, swipeType));
                        activeGestures[swipeType] = true;
                    }
                    break;
                }
            default:
               std::cout<<"Leap Motion gesture not recognized\n";
               break;
        }
    }

    detectPinch(frame, curGestures);

    for(int i=0; i<INVALID_GESTURE; i++)
    {
        activeGestures[i] = curGestures[i];
    }
}

void GestureCapture::detectPinch(Frame frame, bool *curGestures)
{
    if(1==frame.hands()[0].pinchStrength())
    {
        curGestures[PINCH] = true;
        if(!activeGestures[PINCH])
            GestureQueue::getInstance()->push(new GestureMessage(PINCH, NONE));
        activeGestures[PINCH] = true;
    }
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
