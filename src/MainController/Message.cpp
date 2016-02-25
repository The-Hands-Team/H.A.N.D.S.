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


GestureMessage::GestureMessage( GestureType g, Direction d, Handedness h): Message(GESTURE), gesture(g), dir(d), hand(h)
{
}
GestureType GestureMessage::getGesture()
{
    return gesture;
}
Direction GestureMessage::getDir()
{
    return dir;
}
Handedness GestureMessage::getHandedness()
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



FileSystemMessage::FileSystemMessage(std::thread::id id, std::error_condition err, FileManager::FileSystemAction act, fs::path p1, fs::path p2 ) : Message(FILESYSTEM),t_id(id)
                                                                                                                 , errCode(err), action(act)
                                                                                                                 , path1(p1), path2(p2)
{
}
std::thread::id FileSystemMessage::get_t_id()
{
    return t_id;
}
std::error_condition FileSystemMessage::getErrCode()
{
    return errCode;
}
FileManager::FileSystemAction FileSystemMessage::getAction()
{
    return action;
}
fs::path FileSystemMessage::getPath1()
{
    return path1;
}
fs::path FileSystemMessage::getPath2()
{
    return path2;
}

