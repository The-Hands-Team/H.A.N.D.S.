#include <irrlicht/IEventReceiver.h>
#include <irrlicht/Keycodes.h>

#ifndef EVENTLISTENER_HPP
#define EVENTLISTENER_HPP

class EventListener : public irr::IEventReceiver
{
public:
    // This is the one method that we have to implement
    virtual bool OnEvent(const irr::SEvent& event);

    // This is used to check whether a key is being held down
    virtual bool IsKeyDown(irr::EKEY_CODE keyCode) const;

    EventListener();

private:
    // We use this array to store the current state of each key
    bool KeyIsDown[irr::KEY_KEY_CODES_COUNT];
};

#endif //EVENTLISTENER_HPP
