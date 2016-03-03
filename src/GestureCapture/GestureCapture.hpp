
#ifndef GESTURE_CAPTURE
#define GESTURE_CAPTURE

#include "LeapMotion/Leap.h"
#include <map>

void initGesture(bool background);

enum class GestType;
enum class GestDir;
enum class GestHand;

class GestureCapture : public Leap::Listener {
    public:

        GestureCapture(bool);
        ~GestureCapture();

        static GestureCapture* getInstance();

        void mainLoop();

        virtual void onInit(const Leap::Controller&) {};
        virtual void onConnect(const Leap::Controller&);
        virtual void onDisconnect(const Leap::Controller&) {};
        virtual void onExit(const Leap::Controller&) {};
        virtual void onFrame(const Leap::Controller&);
        virtual void onFocusGained(const Leap::Controller&) {};
        virtual void onFocusLost(const Leap::Controller&) {};
        virtual void onDeviceChange(const Leap::Controller&) {};
        virtual void onServiceConnect(const Leap::Controller&) {};
        virtual void onServiceDisconnect(const Leap::Controller&) {};

    private:
        // This should be removed soon anyway
        std::map<GestType, bool> activeGestures {{CIRCLE, false},
                                                 {PINCH, false},
                                                 {GRAB, false},
                                                 {SCREEN_TAP, false},
                                                 {SWIPE, false}};
        void checkHands(Leap::Frame frame, bool *curGestures);

        Leap::Controller controller;
        static GestureCapture* instance;

};

#endif // GESTURE_CAPTURE
