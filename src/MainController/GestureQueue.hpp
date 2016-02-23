#ifndef GESTUREQUEUE_H
#define GESTUREQUEUE_H
#include "FileSystem/FileManager.hpp"
#include <irrlicht/Keycodes.h>
#include <thread>
#include <mutex>
#include <queue>

enum MessageType
{
    GESTURE,
    KEYPRESS,
    FILESYSTEM,
    INVALID_MESSAGE
};

class Message
{
public:
    MessageType getType() {return type;};
    virtual ~Message() {};
protected:
    Message () :type(INVALID_MESSAGE) {};
    Message (MessageType m) : type(m) {};
    MessageType type;
};

enum GestureType {
    CIRCLE,
    PINCH,
    GRAB,
    SCREEN_TAP,
    SWIPE,
    INVALID_GESTURE
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

enum Hand {
    HAND_LEFT,
    HAND_RIGHT
};

//TODO move code to cpp

class GestureMessage : public Message
{
public:
    GestureMessage(GestureType g, Direction d, Hand h): Message(GESTURE),gesture(g),dir(d),hand(h) {};
    GestureType getGesture() { return gesture; };
    Direction getDir() { return dir; };
    Hand getHand() { return hand; };

private:
    GestureType gesture;
    Direction dir;
    Hand hand;
};

class KeyMessage : public Message
{
public:
    KeyMessage(irr::EKEY_CODE k, bool s, bool c, bool p) : Message(KEYPRESS),key(k),shift(s),ctrl(c),pressed(p){};
    irr::EKEY_CODE getKey(){ return key; };
    bool getShift(){ return shift; };
    bool getCtrl(){ return ctrl; };
    bool getPressed(){ return pressed; };

private:
    irr::EKEY_CODE key;
    bool shift;
    bool ctrl;
    bool pressed;
};

class FileSystemMessage : public Message
{
public:
    FileSystemMessage(std::thread::id id, std::error_condition err, FileManager::FileSystemAction act, fs::path p1, fs::path p2 ) : Message(FILESYSTEM),t_id(id)
                                                                                                                 , errCode(err), action(act)
                                                                                                                 , path1(p1), path2(p2){};
    std::thread::id get_t_id(){ return t_id; };
    std::error_condition getErrCode(){ return errCode; };
    FileManager::FileSystemAction getAction(){return action;};
    fs::path getPath1(){return path1;};
    fs::path getPath2(){return path2;};

private:
    std::thread::id t_id;
    std::error_condition errCode;
    FileManager::FileSystemAction action;
    fs::path path1;
    fs::path path2;
};


class GestureQueue {
private:
    std::queue<Message*> gesture_q;
    std::mutex q_mutex;
    static GestureQueue* instance;
public:
    ~GestureQueue();
    static GestureQueue* getInstance();
    void push (Message*);
    Message* pop ();
    Message* peek ();
    int size ();
    std::condition_variable event_cv;
    std::mutex event_m;
};

#endif
