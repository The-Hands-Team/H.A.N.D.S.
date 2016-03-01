#include "MainController/MainController.hpp"
#include "GestureCapture/GestureCapture.hpp"
#include "Graphics/Graphics.hpp"
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;

int threadsReady;

void threadsWait()
{
    std::unique_lock<std::mutex> lck(mtx);
    threadsReady++;
    cv.notify_all();
    cv.wait(lck, []{return 3 == threadsReady;});
}

void mcInit()
{
    MainController* mainController = new MainController();
    threadsWait();
    mainController->mainLoop();
    delete mainController;

}

void gInit()
{
    Graphics* graphics = new Graphics();
    threadsWait();
    graphics->mainLoop();
    delete graphics;
}

void gcInit()
{
    GestureCapture* gestureCapture = new GestureCapture(false);
    threadsWait();
    gestureCapture->mainLoop();
    delete gestureCapture;
}



int main()
{

    threadsReady = 0;
    // Initialize

    // Start Execution
    std::thread t_mc(mcInit);
    std::thread t_g(gInit);
    std::thread t_gc(gcInit);

    // Wait for execution to end
    t_gc.join();
    t_g.join();
    t_mc.join();

    return 0;

}
