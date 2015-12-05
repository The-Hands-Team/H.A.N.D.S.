/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>
#include <cstring>
//#include "..\include\Leap.h"
#include "Leap.h"
#include "GestureEvent.cpp"
#include "../MainController/MainController.hpp"

using namespace Leap;

class SampleListener : public Listener {
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
};

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};
const std::string gestureNames[] = {"Circle", "Key Tap", "Screen Tap", "Swipe"};

void SampleListener::onInit(const Controller& controller) {
  std::cout << "Leap Motion Initialized" << std::endl;
  MainController::getInstance()->initThread();
  std::cout << "Main Controller Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
  std::cout << "Leap Motion Connected" << std::endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  std::cout << "Leap Motion Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
  std::cout << "Leap Motion Exited" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
	// Get the most recent frame and report some basic information
	const Frame frame = controller.frame();
	if(!frame.gestures().isEmpty())
	{
		Leap::GestureList gestures = frame.gestures();
		for(Leap::GestureList::const_iterator gl = gestures.begin(); gl != gestures.end(); gl++)
		{
			switch ((*gl).type()) {
				case Leap::Gesture::TYPE_CIRCLE:
					MainController::getInstance()->pushEvent(new GestureEvent(gestureNames[0], CIRCLE));
					break;
				case Leap::Gesture::TYPE_KEY_TAP:
					MainController::getInstance()->pushEvent(new GestureEvent(gestureNames[1], KEY_TAP));
					break;
				case Leap::Gesture::TYPE_SCREEN_TAP:
					MainController::getInstance()->pushEvent(new GestureEvent(gestureNames[2], SCREEN_TAP));
					break;
				case Leap::Gesture::TYPE_SWIPE:
					MainController::getInstance()->pushEvent(new GestureEvent(gestureNames[3], SWIPE));
					break;
				default:
					std::cout<<"Leap Motion gesture not recognized\n";
					break;
			}
		}
	}
}

void SampleListener::onFocusGained(const Controller& controller) {
  std::cout << "Leap Motion Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
  std::cout << "Leap Motion Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
  std::cout << "Leap Motion Device Changed" << std::endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    std::cout << "id: " << devices[i].toString() << std::endl;
    std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
  }
}

void SampleListener::onServiceConnect(const Controller& controller) {
  std::cout << "Leap Motion Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
  std::cout << "Leap Motion Service Disconnected" << std::endl;
}

int main(int argc, char** argv) {
  // Create a sample listener and controller
  SampleListener listener;
  Controller controller;

  // Have the sample listener receive events from the controller
  controller.addListener(listener);

  if (argc > 1 && strcmp(argv[1], "--bg") == 0)
    controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

  // Keep this process running until Enter is pressed
  std::cout << "Press Enter to quit..." << std::endl;
  std::cin.get();

  // Remove the sample listener when done
  controller.removeListener(listener);

  return 0;
}
