
#ifndef GESTURE_CAPTURE
#define GESTURE_CAPTURE

#include "LeapMotion/Leap.h"

void initGesture(bool background);

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

enum Handedness {
    LEFT_HAND,
    RIGHT_HAND
};

using namespace Leap;

class GestureCapture : public Listener {
    public:
        GestureCapture(bool);
        ~GestureCapture();

        static GestureCapture* getInstance();

        void mainLoop();

        virtual void onInit(const Controller&) {};
        virtual void onConnect(const Controller&);
        virtual void onDisconnect(const Controller&) {};
        virtual void onExit(const Controller&) {};
        virtual void onFrame(const Controller&);
        virtual void onFocusGained(const Controller&) {};
        virtual void onFocusLost(const Controller&) {};
        virtual void onDeviceChange(const Controller&) {};
        virtual void onServiceConnect(const Controller&) {};
        virtual void onServiceDisconnect(const Controller&) {};

    private:
        bool activeGestures[INVALID_GESTURE]= { 0 };
        void checkHands(Frame frame, bool *curGestures);

        Controller controller;
        static GestureCapture* instance;

};

#endif // GESTURE_CAPTURE
