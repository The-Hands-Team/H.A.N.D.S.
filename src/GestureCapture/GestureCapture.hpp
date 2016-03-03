
#ifndef GESTURE_CAPTURE
#define GESTURE_CAPTURE

#include "LeapMotion/Leap.h"

void initGesture(bool background);

enum class GestureType;
enum class GestureDir;
enum class GestureHand;

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
        bool activeGestures[INVALID_GESTURE]= { 0 };
        void checkHands(Leap::Frame frame, bool *curGestures);

        Leap::Controller controller;
        static GestureCapture* instance;

};

#endif // GESTURE_CAPTURE
