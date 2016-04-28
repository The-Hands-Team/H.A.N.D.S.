
#ifndef GESTURE_CAPTURE
#define GESTURE_CAPTURE

#include "LeapMotion/Leap.h"
#include "Hand.hpp"
#include <bitset>
#include <mutex>

void initGesture(bool background);

enum class GestType {
    CIRCLE,
    PINCH,
    GRAB,
    SCREEN_TAP,
    SWIPE,
    OPEN,
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
constexpr inline std::size_t operator+ (GestType i){ return static_cast<std::size_t>(i); }

using GestFlags = std::bitset<+GestType::INVALID_GESTURE>;

class GestureCapture : public Leap::Listener {
    public:

        GestureCapture(bool);
        ~GestureCapture();

        static GestureCapture* getInstance();
        std::vector<Hand> getHands();

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
        GestFlags activeLeftGestures;
        GestFlags activeRightGestures;
        //bool activePinchHand;
        //bool activeGrabHand;
        std::mutex handLock;
        void checkHands( const Leap::Frame& frame, GestFlags& curLeftGestures, GestFlags& curRightGestures);
        float pinchStrength(Leap::Hand h);

        Leap::Controller controller;
        static GestureCapture* instance;
        std::vector<Hand> handsInFrame;

};

#endif // GESTURE_CAPTURE
