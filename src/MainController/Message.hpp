#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <irrlicht/Keycodes.h>
#include <thread>
#include <system_error>
#include "GestureCapture/GestureCapture.hpp"



class Message
{
public:
    enum MessageType
    {
        GESTURE,
        KEYPRESS,
        FILESYSTEM,
        INVALID_MESSAGE
    };

    MessageType getType();
    Message();
    virtual ~Message();
protected:
    Message (MessageType m);
    MessageType type;
};

class GestureMessage : public Message
{
public:
    GestureMessage(GestType g, GestDir d, GestHand h);
    GestType getGesture();
    GestDir getDir();
    GestHand getHandedness();

private:
    GestType gesture;
    GestDir dir;
    GestHand hand;
};

class KeyMessage : public Message
{
public:
    KeyMessage(irr::EKEY_CODE k, bool s, bool c, bool p);
    irr::EKEY_CODE getKey();
    bool getShift();
    bool getCtrl();
    bool getPressed();

private:
    irr::EKEY_CODE key;
    bool shift;
    bool ctrl;
    bool pressed;
};

#endif //MESSAGE_HPP
