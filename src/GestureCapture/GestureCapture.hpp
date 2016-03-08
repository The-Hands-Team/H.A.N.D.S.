
#ifndef GESTURE_CAPTURE
#define GESTURE_CAPTURE

#include "LeapMotion/Leap.h"
#include <bitset>

void initGesture(bool background);

enum class GestType {
    CIRCLE,
    PINCH,
    GRAB,
    SCREEN_TAP,
    SWIPE,
    INVALID_GESTURE
};

enum class GestDir {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

enum class GestHand {
    LEFT,
    RIGHT
};

//Temporary mesure. Should be removed soon
constexpr std::size_t operator+ (GestType i){ return static_cast<std::size_t>(i); }

using GestFlags = std::bitset<+GestType::INVALID_GESTURE>;

class GestureCapture : public Leap::Listener {
    public:

        GestureCapture(bool);
        ~GestureCapture();

        static GestureCapture* getInstance();

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
        GestFlags activeGestures;
        void checkHands(Leap::Frame frame, GestFlags& curGestures);

        Leap::Controller controller;
        static GestureCapture* instance;

};

#endif // GESTURE_CAPTURE
