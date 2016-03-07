#include "MainController.hpp"
#include <iostream>
#include <errno.h>

#include "Graphics/Graphics.hpp"
#include "GestureCapture/GestureCapture.hpp"

MainController* MainController::curInstance = nullptr;

MainController::MainController()
{
    curInstance = this;
    ignore_new = false;
    GestureQueue::getInstance();

    new_dir_i = 0;
    cur_path = fs::path("tests/test_dir");
}

MainController::~MainController()
{
    curInstance = nullptr;
}

MainController* MainController::getInstance()
{
    return curInstance;
}

void MainController::mainLoop()
{
    GestureQueue* event_q = GestureQueue::getInstance();
    Message* ev;
    updateDirectory(cur_path);

    while(true)
    {
        sendCurrentPath();
        //got something in queue

        ev = event_q->pop();
        processEvent(ev);
    }
}

void MainController::updateDirectory(fs::path new_dir)
{
    if (fs::is_directory(new_dir))
    {
        new_dir_contents.clear();
        //set us on the new_dir
        //get the iterator
        fs::directory_iterator d_it = fs::directory_iterator(new_dir);
        int i = 0;
        new_dir_i = i;
        //iterate through
        clearSelected();
        while (d_it != fs::end(d_it))
        {
            //put the biz in there
            new_dir_contents.push_back(*d_it);
            if (d_it->path() == cur_path)
            {
                new_dir_i = i;
            }
            i++;
            d_it++;
        }
        cur_path = new_dir;
        //update graphiczz
        sendCurrentPath();
    }
}

void MainController::chdirUp()
{
    updateDirectory(cur_path.parent_path());
}

void MainController::chdirDown()
{
    updateDirectory(new_dir_contents[new_dir_i].path());
}

void MainController::iterateForward()
{
    new_dir_i = (new_dir_i + 1) % new_dir_contents.size();
}

void MainController::iterateBack()
{
    new_dir_i = (new_dir_i + new_dir_contents.size() - 1) % new_dir_contents.size();
}


void MainController::select()
{
    fs::path selection = curEntry().path();
    if( 0 == selected.count( selection ) )
    {
        selected.emplace( selection );
    }
    else
    {
        selected.erase( selection );
    }
}

void MainController::copyInto(fs::path dest)
{
    std::vector<fs::path> source;
    std::vector<fs::path> destination;
    if (fs::is_directory(dest))
    {
        for( auto const v : selected )
        {
            fs::path dest_name = dest / v.filename();
            if( fs::exists(dest_name) ) dest_name += "_copy";
            source.emplace_back( v );
            destination.emplace_back( dest_name );
        }

        if( 0 < source.size() )
        {
            FileManager::getInstance()->copyFiles(source,destination);
        }

        clearSelected();
    }
}

void MainController::moveInto(fs::path dest)
{
    std::vector<fs::path> source;
    std::vector<fs::path> destination;
    if (fs::is_directory(dest))
    {
        for( auto const v : selected )
        {
            fs::path dest_name = dest / v.filename();
            if( fs::exists(dest_name) ) dest_name += "_copy";
            source.emplace_back( v );
            destination.emplace_back( dest_name );
        }

        if( 0 < source.size() )
        {
            FileManager::getInstance()->moveFiles(source,destination);
        }

        clearSelected();
    }
}

void MainController::clearSelected()
{
    selected.clear();
}

fs::directory_entry MainController::curEntry()
{
    return new_dir_contents[new_dir_i];
}

void MainController::processEvent(Message* m)
{

    if ( Message::GESTURE == m->getType() )
    {
        GestureMessage* ge = dynamic_cast<GestureMessage*>(m);
        switch (ge->getGesture())
        {
        case GestType::PINCH:
            if( GestHand::RIGHT == ge->getHandedness() )
            {
                select();
                break;
            }
            else
            {
                copyInto( cur_path );
            }
        case GestType::SCREEN_TAP:
            if( GestHand::RIGHT == ge->getHandedness() )
            {
                std::cout << cur_path << std::endl;
            }
            break;
        case GestType::CIRCLE:
            break;
        case GestType::SWIPE:
            if( GestHand::RIGHT == ge->getHandedness() )
            {
                switch (ge->getDir())
                {
                case GestDir::UP:
                    chdirUp();
                    break;
                case GestDir::DOWN:
                    chdirDown();
                    break;
                case GestDir::RIGHT:
                    iterateForward();
                    break;
                case GestDir::LEFT:
                    iterateBack();
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch (ge->getDir())
                {
                case GestDir::UP:
                    copyInto(cur_path.parent_path());
                    break;
                case GestDir::DOWN:
                    copyInto(curEntry());
                    break;
                case GestDir::RIGHT:
                    moveInto(cur_path.parent_path());
                    break;
                case GestDir::LEFT:
                    moveInto(curEntry());
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            std::cout << "Gesture not supported\n";
            break;
        }
    }
    else if( Message::KEYPRESS == m->getType() )
    {
        KeyMessage* ke = dynamic_cast<KeyMessage*>(m);
        if(ke->getPressed())
            switch(ke->getKey())
            {
            case irr::EKEY_CODE::KEY_KEY_C:
                if( ke->getShift() )
                {
                    copyInto(cur_path.parent_path());
                }
                else if ( ke->getCtrl() )
                {
                    copyInto(curEntry());
                }
                else
                {
                    copyInto(cur_path);
                }
                break;
            case irr::EKEY_CODE::KEY_KEY_M:
                if( ke->getShift() )
                {
                    moveInto(cur_path.parent_path());
                }
                else if ( ke->getCtrl() )
                {
                    moveInto(curEntry());
                }
                else
                {
                    moveInto(cur_path);
                }
                break;
            case irr::EKEY_CODE::KEY_KEY_S:
                {
                    select();
                    break;
                }
            case irr::EKEY_CODE::KEY_KEY_D:
                {
                    std::cout << ke->getShift() << ke->getCtrl() << std::endl;
                }
                break;
            case irr::EKEY_CODE::KEY_UP:
                chdirUp();
                break;
            case irr::EKEY_CODE::KEY_DOWN:
                chdirDown();
                break;
            case irr::EKEY_CODE::KEY_RIGHT:
                iterateForward();
                break;
            case irr::EKEY_CODE::KEY_LEFT:
                iterateBack();
                break;
            default:
                break;
            }
    }
    else if( Message::FILESYSTEM == m->getType() )
    {
        FileSystemMessage* fe = dynamic_cast<FileSystemMessage*>(m);
        switch( fe->getErrCode().value() )
        {
            case 0:
                FileManager::getInstance()->joinThread(fe->get_t_id());
                updateDirectory(cur_path);
                break;
            case EIO:
                std::cerr << fe->getErrCode().value() << fe->getErrCode().message() << " " << fe->getPath1() << " " << fe->getPath2() << std::endl;
                FileManager::getInstance()->replyToError( fe->get_t_id(), HandleErrorCommand::IGNORE );
                updateDirectory(cur_path);
                break;
            default:
                std::cerr << fe->getErrCode().value() << fe->getErrCode().message() << " " << fe->getPath1() << " " << fe->getPath2() << std::endl;
                FileManager::getInstance()->replyToError( fe->get_t_id(), HandleErrorCommand::TERMINATE );
                updateDirectory(cur_path);
                break;
                //we don't know;
        }
    }
    delete m;
}

void MainController::sendCurrentPath()
{
    // TODO Also keep a copy of the list for ourselves, possibly with more information
    std::vector<DirObject> objs;

    for ( unsigned int i = 0; i < new_dir_contents.size(); i++ )
    {
        objs.emplace_back( fs::is_directory(new_dir_contents[i].path())?'d':'f'
                         , 0.25f*(i/5),0.25f*(i%5)
                         , new_dir_contents[i].path().filename().wstring()
                         , i == new_dir_i
                         , selected.count( new_dir_contents[i].path() ) );
    }

    Graphics::getInstance()->newObjects(objs);

}
