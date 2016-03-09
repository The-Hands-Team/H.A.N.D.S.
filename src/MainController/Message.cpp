#include "Message.hpp"

Message::MessageType Message::getType()
{
    return type;
}
Message::~Message()
{

}
Message::Message() : type(INVALID_MESSAGE)
{
}
Message::Message (Message::MessageType m) : type(m)
{
}


GestureMessage::GestureMessage( GestType g, GestDir d, GestHand h): Message(GESTURE), gesture(g), dir(d), hand(h)
{
}
GestType GestureMessage::getGesture()
{
    return gesture;
}
GestDir GestureMessage::getDir()
{
    return dir;
}
GestHand GestureMessage::getHandedness()
{
    return hand;
}



KeyMessage::KeyMessage(irr::EKEY_CODE k, bool s, bool c, bool p) : Message(KEYPRESS),key(k),shift(s),ctrl(c),pressed(p){}
irr::EKEY_CODE KeyMessage::getKey()
{
    return key;
}
bool KeyMessage::getShift()
{
    return shift;
}
bool KeyMessage::getCtrl()
{
    return ctrl;
}
bool KeyMessage::getPressed()
{
    return pressed;
}
