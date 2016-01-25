#include "MainController.hpp"
#include <iostream>
#include <chrono>

MainController* MainController::curInstance = nullptr;

MainController::MainController()
{
    ignore_new = false;
}

MainController::~MainController()
{
}

MainController* MainController::getInstance()
{
    if (curInstance == nullptr)
    {
        curInstance = new MainController();
    }

    return curInstance;
}

int main(int argc, char** argv)
{
    std::thread graphics(initGraphics);
    std::thread gesture(initGesture, false);
    gesture.detach();
    graphics.detach();
    MainController::getInstance()->mainLoop();
}

void MainController::mainLoop()
{
    GestureQueue* event_q = GestureQueue::getInstance();
    cur_path = fs::path("tests/test_dir");
    dir_it = fs::directory_iterator(cur_path);

    while (true)
    {
        sendCurrentPath();
        //got something in queue
        Message* ev = event_q->pop();
        //TODO: ACTUALLY PROCESS THE EVENT
        processEvent(ev);
        while (event_q->size() != 0)
        {
            ev = event_q->pop();
            processEvent(ev);
        }
    }
}

void MainController::processEvent(Message* m)
{
    //std::cout << "This is the part where an event gets processed! It was: " << ge->getName() << std::endl;
    std::cout << m->getType() << std::endl;

    if (GESTURE == m->getType())
    {
        GestureMessage* ge = dynamic_cast<GestureMessage*>(m);
        std::cout << ge->getGesture() << std::endl;
        switch (ge->getGesture())
        {
        case KEY_TAP:
        {
            break;
            fs::path dest = dir_it->path();
            dest += fs::path("_copy");
            FileManager::getInstance()->copyFile(dir_it->path(),dest);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            sendCurrentPath();
            break;
        }
        case SCREEN_TAP:
            std::cout << fs::absolute(cur_path) << std::endl;
            break;
        case CIRCLE:
            break;
        case SWIPE:
            switch (ge->getDir())
            {
            case UP:
                if (cur_path.parent_path() != "")
                {
                    fs::path parent = cur_path.parent_path();
                    dir_it = fs::directory_iterator(parent);
                    while (dir_it != fs::end(dir_it)
                            && dir_it->path() != cur_path)
                        dir_it++;
                    cur_path = parent;
                    sendCurrentPath();
                }
                break;
            case DOWN:
                if (fs::is_directory(dir_it->path()))
                {
                    cur_path = dir_it->path();
                    dir_it = fs::directory_iterator(cur_path);
                    sendCurrentPath();
                }
                std::cout << cur_path << std::endl;
                break;
            case RIGHT:
                dir_it++;
                if (dir_it == fs::end(dir_it))
                {
                    dir_it = fs::directory_iterator(cur_path);
                }
                std::cout << dir_it->path() << std::endl;
                break;
            default:
                break;
            }
            break;
        default:
            std::cout << "Gesture not supported\n";
            break;
        }
    }
    else if(KEYPRESS == m->getType())
    {
        KeyMessage* ke = dynamic_cast<KeyMessage*>(m);
        if(ke->getPressed())
            switch(ke->getKey())
            {
            case irr::EKEY_CODE::KEY_KEY_C:
            {
                fs::path dest = dir_it->path();
                dest += fs::path("_copy");
                FileManager::getInstance()->copyFile(dir_it->path(),dest);
                sendCurrentPath();
                break;
            }
            case irr::EKEY_CODE::KEY_UP:
                if (cur_path.parent_path() != "")
                {
                    fs::path parent = cur_path.parent_path();
                    dir_it = fs::directory_iterator(parent);
                    while (dir_it != fs::end(dir_it)
                    && dir_it->path() != cur_path)
                        dir_it++;
                    cur_path = parent;
                    sendCurrentPath();
                }
                break;
            case irr::EKEY_CODE::KEY_DOWN:
                if (fs::is_directory(dir_it->path()))
                {
                    cur_path = dir_it->path();
                    dir_it = fs::directory_iterator(cur_path);
                    sendCurrentPath();
                }
                std::cout << cur_path << std::endl;
                break;
            case irr::EKEY_CODE::KEY_RIGHT:
                dir_it++;
                if (dir_it == fs::end(dir_it))
                {
                    dir_it = fs::directory_iterator(cur_path);
                }
                std::cout << dir_it->path() << std::endl;
                break;
            default:
                break;
            }
    }
    else if(KEYPRESS == m->getType())
    {
        KeyMessage* fe = dynamic_cast<FileSystemMessage*>(m);
        switch( fe->getErrCode() )
        {
            case 0:
                sendCurrentPath();
                break;
            default:
                //we don't know;
        }
    }
    delete m;
}

void MainController::sendCurrentPath()
{
    size_t length = 0;
    // TODO Also keep a copy of the list for ourselves, possibly with more information
    for( fs::directory_iterator it (cur_path); it != fs::end(it); it++) length++;

    dirObject* objs = new dirObject[length];

    size_t i = 0;
    for( fs::directory_iterator it (cur_path); i<length; it++, i++)
    {
        const std::wstring name = it->path().filename().wstring();
        objs[i] = dirObject(fs::is_directory(it->path())?'d':'f',1.5f*(i/4)+0.5,1.5f*(i%4)+0.5,name.data(), *it == *dir_it);
    }

    newObjects(objs, length);

}
