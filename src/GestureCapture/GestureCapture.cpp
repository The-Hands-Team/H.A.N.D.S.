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

GestureCapture* GestureCapture::instance = nullptr;

void GestureCapture::onConnect(const Leap::Controller& controller) {
  controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
  controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
}

void GestureCapture::onFrame(const Leap::Controller& controller) {
    // Get the most recent frame and report some basic information
    const Leap::Frame frame = controller.frame();
    const Leap::Frame prevFrame = controller.frame(1);
    //Initialize all gestures to false
    bool curGestures[INVALID_GESTURE] = { 0 };

    // TODO don't ignore when the same gesture is made simultaneously by different fingers

    Leap::GestureList gestures = frame.gestures();
    for(Leap::GestureList::const_iterator gl = gestures.begin(); gl != gestures.end(); gl++)
    {
        Handedness hand = (*(*gl).hands().begin()).isRight() ? RIGHT_HAND : LEFT_HAND;
        switch ((*gl).type()) {
            case Leap::Gesture::TYPE_CIRCLE:
                curGestures[CIRCLE] = true;
                if(!activeGestures[CIRCLE])
                    GestureQueue::getInstance()->push(new GestureMessage(CIRCLE, NONE, hand));
                activeGestures[CIRCLE] = true;
                break;
            case Leap::Gesture::TYPE_SCREEN_TAP:
                curGestures[SCREEN_TAP] = true;
                if(!activeGestures[SCREEN_TAP])
                    GestureQueue::getInstance()->push(new GestureMessage(SCREEN_TAP, NONE, hand));
                activeGestures[SCREEN_TAP] = true;
                break;
            case Leap::Gesture::TYPE_SWIPE:
            {
                    Leap::SwipeGesture sg = *gl;
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
                            GestureQueue::getInstance()->push(new GestureMessage(SWIPE, swipeType, hand));
                        activeGestures[swipeType] = true;
                    }
                    break;
                }
            default:
               std::cout<<"Leap Motion gesture not recognized\n";
               break;
        }
    }

    checkHands(frame, curGestures);

    for(int i=0; i<INVALID_GESTURE; i++)
    {
        activeGestures[i] = curGestures[i];
    }
}

void GestureCapture::checkHands(Leap::Frame frame, bool *curGestures)
{
  Leap::HandList hands = frame.hands();
  for(Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); hl++)
  {
    float pinchStr = (*hl).pinchStrength();
    float grabStr = (*hl).grabStrength();
    if(1==grabStr && 1>pinchStr)
    {
        curGestures[GRAB] = true;
        if(!activeGestures[GRAB])
            GestureQueue::getInstance()->push(new GestureMessage(GRAB, NONE, (((*hl).isRight()) ? RIGHT_HAND : LEFT_HAND )));
        activeGestures[GRAB] = true;
    }
    else if(1==pinchStr && .7>grabStr)
    {
        curGestures[PINCH] = true;
        if(!activeGestures[PINCH])
            GestureQueue::getInstance()->push(new GestureMessage(PINCH, NONE, (((*hl).isRight()) ? RIGHT_HAND : LEFT_HAND )));
        activeGestures[PINCH] = true;
    }
  }
}

GestureCapture::GestureCapture(bool background) : Listener()
{
    instance = this;

    if (background)
        controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

   controller.addListener(*instance);
}

GestureCapture::~GestureCapture()
{
   controller.removeListener(*instance);
   instance = nullptr;
}

GestureCapture* GestureCapture::getInstance()
{
    return instance;
}

void GestureCapture::mainLoop()
{
  while(true);
}
