#include "MainController.hpp"
#include <future>
#include <iostream>
#include <errno.h>
#include <memory>

#include "Graphics/Graphics.hpp"
#include "GestureCapture/GestureCapture.hpp"
#include "FileSystem/FileSystemMessage.hpp"

MainController* MainController::curInstance = nullptr;

MainController::MainController()
: gq()  // This one should be first
, gc(false)
, fm()
, graphics_t(Graphics::initGraphics)
, ignore_new(false)
, new_dir_i(0)
, cur_path("tests/test_dir")
{
    curInstance = this;
    
    
    
    //blocks until Graphics Initialized
    Graphics::waitForInit();
    

    updateDirectory(cur_path);

}

MainController::~MainController()
{
    Graphics::killGraphics(); //Just in case
    graphics_t.join();
    curInstance = nullptr;
}

MainController* MainController::getInstance()
{
    return curInstance;
}

void MainController::mainLoop()
{
    std::unique_ptr<Message> ev;
    
    while(Graphics::getInstance())
    {
                try 
                {
        sendCurrentPath();
        //got something in queue

        ev = gq.pop();
        processEvent(ev);
                }
                catch (const std::exception& e)
                {
                        std::cerr << e.what() << std::endl;
                }
    }
}

bool MainController::isLong(enum GestType gt)
{
    return gt == GestType::GRAB || gt == GestType::PINCH;
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
    if( cur_path.parent_path() != fs::path( "tests" ) )
        updateDirectory(cur_path.parent_path());
}

void MainController::chdirDown()
{
    if (hasTarget())
    {
        updateDirectory(curEntry().path());
    }
}

void MainController::iterateForward()
{
    new_dir_i = (new_dir_i + 1) % new_dir_contents.size();
}

void MainController::iterateBack()
{
    new_dir_i = (new_dir_i + new_dir_contents.size() - 1) % new_dir_contents.size();
}

void MainController::select() // TODO remove itterator, take a string
{
    if (hasTarget())
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
            fm.copyFiles(source,destination, fs::copy_options::recursive);
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
            fm.moveFiles(source,destination, fs::copy_options::recursive);
        }

        clearSelected();
    }
}

void MainController::deleteSelected()
{
    std::vector<fs::path> toDel;
    for( auto const v : selected )
    {
        toDel.emplace_back( v );
    }

    if( 0 < toDel.size() )
    {
        moveInto( fs::path("tests/TRASH") );
    }

    clearSelected();
}

void MainController::clearSelected()
{
    selected.clear();
}

void MainController::clearClipboard()
{
    fs::path clipboard( "tests/CLIPBOARD" );
    if( fs::exists( clipboard ) && fs::is_directory( clipboard ) )
    {
        for( auto& dir_e : fs::directory_iterator(clipboard) )
        {
            fs::remove_all( dir_e );
        }
    }
}

void MainController::pasteFromClipboard()
{
    fs::path clipboard( "tests/CLIPBOARD" );
    if( fs::exists( clipboard ) && fs::is_directory( clipboard ) )
    {
        std::vector<fs::path> fromPaste;
        std::vector<fs::path> toPaste;
        for( auto& dir_e : fs::directory_iterator(clipboard) )
        {
            fromPaste.emplace_back(dir_e);
            toPaste.emplace_back( cur_path / dir_e.path().filename() );
        }
        
        fm.copyFiles( fromPaste, toPaste, fs::copy_options::recursive );
    }
}


bool MainController::hasTarget()
{
    unsigned int i = pathToIndex(Graphics::getInstance()->currentHighlightedPath());
    if ((unsigned int) -1 == i)
    {
        return false;
    }
    else
    {
        return true;
    }
}

fs::directory_entry MainController::curEntry()
{
        //instead of new_dir_i, call da biz
    unsigned int i = pathToIndex(Graphics::getInstance()->currentHighlightedPath());
    if ((unsigned int) -1 == i)
    {
        std::cerr << "Attempted to access element when nothing is highlighted!\n";
        return fs::directory_entry();
    }
    return new_dir_contents[i];
}

void MainController::handleGestureMessage(std::unique_ptr<GestureMessage> ge)
{
        std::string gestStrings[] = {"circle", "pinch", "grab", "screen tap", "swipe", "open", "invalid"};
        std::cout << "Got gesture: " << gestStrings[(int) ge->getGesture()] << (ge->isStopping() ? " stop\n" : " start\n");
        if (ge->isStopping())
        {
            //std::cout << "Stopping! " << (GestHand::RIGHT == ge->getHandedness() ? "right" : "left")<< std::endl;
        }
        //lonk or short
        //the handz
                //keep track of hands and their gestures
        //
        if (!isLong(ge->getGesture()))
        {
            curGests[GestHand::RIGHT == ge->getHandedness() ? RIGHT : LEFT] = GestType::INVALID_GESTURE;
            if (ge->isStopping())
            {
                return;
            }
        }
        else
        {
            if (ge->isStopping())
            {
                
            }
            //TODO: verify that we can assume only 1 gesture per hand at a time
            curGests[GestHand::RIGHT == ge->getHandedness() ? RIGHT : LEFT] = ge->isStopping() ? GestType::INVALID_GESTURE : ge->getGesture();
        }
        switch (ge->getGesture())
        {
        case GestType::PINCH:
                if( GestHand::RIGHT == ge->getHandedness() )
                {
                    if( GestType::GRAB == curGests[LEFT] && ge->isStopping() )
                    {
                        #ifdef __linux
                            fs::path p = curEntry().path();
                            if( !p.empty() && !fs::is_directory( p ) )
                            {
                                std::string command = "xdg-open " + p.native();
                                system(command.c_str());
                                std::cout << command << std::endl;
                            }
                            else
                        #endif //__linux
                            {
                                chdirDown();
                            }
                        break;
                    }
                    else if ( GestType::GRAB != curGests[LEFT] && !ge->isStopping() )
                    {
                        select();
                        break;
                    }
                }
        case GestType::SCREEN_TAP:
                if( GestHand::RIGHT == ge->getHandedness() )
                {
                        std::cout << cur_path << std::endl;
                }
                break;
        case GestType::CIRCLE:
            if( GestHand::RIGHT == ge->getHandedness() )
            {
                        if( GestDir::RIGHT == ge->getDir() )
                        {
                                fm.redo();
                        }
                        else
                        {
                                fm.undo();
                        }
                }
                break;
        case GestType::GRAB:
                if( GestHand::RIGHT == ge->getHandedness() )
                {
                    std::cout << "GRAB " << (ge->isStopping() ? "stop" : "start") << std::endl;
                    
                }
                else if (!ge->isStopping())
                {
                    std::cout << "Left Grab\n";
                }
                if( GestType::GRAB == curGests[LEFT] && GestType::GRAB == curGests[RIGHT] )
                {
                   deleteSelected();
                }
                break;
        case GestType::SWIPE:
                if( GestHand::LEFT == ge->getHandedness() )
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
                                std::cout << "paste" << std::endl;
                                pasteFromClipboard();
                                break;
                        case GestDir::LEFT:
                                clearClipboard();
                                std::cout << "copy to clipboard" << std::endl;
                                copyInto(fs::path("tests/CLIPBOARD"));
                                break;
                        default:
                                break;
                        }
                }
                else
                {
                        /*switch (ge->getDir())
                        {
                        case GestDir::UP:
                            if( cur_path.parent_path() != fs::path( "tests" ) )
                                copyInto(cur_path.parent_path());
                            break;
                        case GestDir::DOWN:
                            copyInto(curEntry());
                            break;
                        case GestDir::RIGHT:
                            if( cur_path.parent_path() != fs::path( "tests" ) )
                                moveInto(cur_path.parent_path());
                            break;
                        case GestDir::LEFT:
                            moveInto(curEntry());
                            break;
                        default:
                            break;
                        }*/
                }
                break;
        default:
                std::cout << "Gesture not supported\n";
                break;
        }
}

void MainController::processEvent(std::unique_ptr<Message>& m)
{
    // TODO Break into smaller functions i.e. handleGestureMessage
    switch (m->getType())
    {
        case Message::INVALID_MESSAGE:
        {
            Graphics::killGraphics(); //This will terminate the program
        }
        break;
        case Message::GESTURE:
        {
            handleGestureMessage(dynamic_unique_move<GestureMessage>(std::move(m)));
                }
        break;
        case Message::KEYPRESS:
        {
            auto ke = dynamic_unique_move<KeyMessage>(std::move(m));
            if(ke->getPressed())
                switch(ke->getKey())
                {
                                case irr::EKEY_CODE::KEY_KEY_U:
                                        fm.undo();
                                        break;
                                case irr::EKEY_CODE::KEY_KEY_R:
                                        fm.redo();
                                        break;
                case irr::EKEY_CODE::KEY_KEY_C:
                    if( ke->getShift() )
                    {
                        if( cur_path.parent_path() != fs::path( "tests" ) )
                            copyInto(cur_path.parent_path());
                    }
                    else if ( ke->getCtrl() )
                    {
                        if (hasTarget())
                        {
                            copyInto(curEntry());
                        }
                    }
                    else
                    {
                        copyInto(cur_path);
                    }
                    break;
                case irr::EKEY_CODE::KEY_KEY_M:
                    if( ke->getShift() )
                    {
                        if( cur_path.parent_path() != fs::path( "tests" ) )
                            moveInto(cur_path.parent_path());
                    }
                    else if ( ke->getCtrl() )
                    {
                        if (hasTarget())
                        {
                            moveInto(curEntry());
                        }
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
                        moveInto( fs::path("tests/TRASH") );
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
        break;
        case Message::FILESYSTEM:
        {
            auto fe = dynamic_unique_move<FileSystemMessage>(std::move(m));

            switch( fe->getErrCode().value() )
            {
                case 0:
                    fe->getPromise().set_value( HandleErrorCommand::NO_ERROR );
                    updateDirectory(cur_path);
                    break;
                case EEXIST:
                    {
                            fs::path dest = fe->getPath2();
                            dest += fs::path("_copy");
                        fs::paths from(1, fe->getPath1());
                        fs::paths to(1, dest);
                        if( FileSystemAction::COPY == fe->getAction() )
                        {
                            fm.copyFiles( from, to, fs::copy_options::recursive  );
                        }
                        else if( FileSystemAction::COPY == fe->getAction() )
                        {
                            fm.moveFiles( from, to, fs::copy_options::recursive );
                        }
                        
                        fe->getPromise().set_value(HandleErrorCommand::IGNORE);
                        
                    }
                    break;
                case EIO:
                    // This happens when copying empty file.
                    if( FileSystemAction::COPY == fe->getAction() &&
                        fs::exists( fe->getPath2() ) &&
                        fs::exists( fe->getPath1() ) &&
                        !fs::is_directory( fe->getPath1() ) &&
                        !fs::is_directory( fe->getPath2() ) &&
                        fs::file_size(fe->getPath1()) == fs::file_size(fe->getPath2()))
                    {
                        std::cout << fs::file_size(fe->getPath1()) << std::endl;
                        auto stats = fs::status(fe->getPath1());
                        fs::permissions(fe->getPath2(), stats.permissions());
                        fe->getPromise().set_value( HandleErrorCommand::IGNORE );
                        updateDirectory(cur_path);
                        break;
                    }
                    // Break intentionally ommitted
                default:
                    fe->prettyPrintMessage(std::cerr);
                    fe->getPromise().set_value( HandleErrorCommand::TERMINATE );
                    updateDirectory(cur_path);
                    break;
                    //we don't know;
            }
        }
        break;
    }
}

void MainController::sendCurrentPath()
{
    // TODO Also keep a copy of the list for ourselves, possibly with more information
    std::vector<DirObject> objs{};

    for ( unsigned int i = 0; i < new_dir_contents.size(); i++ )
    {
        objs.emplace_back( fs::is_directory(new_dir_contents[i].path())?'d':'f'
                         , 0, 0
                         , new_dir_contents[i].path().filename().wstring()
                         , 0//i == new_dir_i
                         , selected.count( new_dir_contents[i].path() ) );
    }

    Graphics* g = Graphics::getInstance();
    if( g )
    {
        g->newObjects(std::move(objs));
    }
}

unsigned int MainController::pathToIndex(std::wstring p)
{
        if (p.empty())
        {
        return -1;
        }
    for ( unsigned int i = 0; i < new_dir_contents.size(); i++)
        {
        if (p == new_dir_contents[i].path().filename().wstring())
                {
            return i;
            }
        }
        return -1;
}

// Start it all.
int main()
{
    MainController mc{};

    mc.mainLoop();

    return 0;

}
