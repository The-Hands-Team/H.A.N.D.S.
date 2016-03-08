
#include "irrlicht/irrTypes.h"
#include "EventListener.hpp"
#include "MainController/GestureQueue.hpp"


bool EventListener::OnEvent(const irr::SEvent& event)
{
    // Remember whether each key is down or up
    if (event.EventType == irr::EET_KEY_INPUT_EVENT)
    {
        if( event.KeyInput.PressedDown != KeyIsDown[event.KeyInput.Key])
            GestureQueue::getInstance()->push(new KeyMessage(event.KeyInput.Key,event.KeyInput.Shift,event.KeyInput.Control,event.KeyInput.PressedDown));
        KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
    }

    return false;
}

// This is used to check whether a key is being held down
bool EventListener::IsKeyDown(irr::EKEY_CODE keyCode) const
{
    return KeyIsDown[keyCode];
}

EventListener::~EventListener()
{
    GestureQueue::getInstance()->push(new Message());
}
