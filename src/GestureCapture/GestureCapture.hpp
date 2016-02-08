
#ifndef GESTURE_CAPTURE
#define GESTURE_CAPTURE

using namespace Leap;

class GestureCapture : public Listener {
    public:

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
        void detectPinch(Frame frame, bool *curGestures);

    private:
        bool activeGestures[INVALID_GESTURE]= { 0 };

};

#endif // GESTURE_CAPTURE
