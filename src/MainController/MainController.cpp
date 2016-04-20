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
    GestureQueue* event_q = GestureQueue::getInstance();
    std::unique_ptr<Message> ev;
    
    while(Graphics::getInstance())
    {
		try 
		{
        sendCurrentPath();
        //got something in queue

        ev = event_q->pop();
        processEvent(ev);
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
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

void MainController::clearSelected()
{
    selected.clear();
}

fs::directory_entry MainController::curEntry()
{
	//instead of new_dir_i, call da biz
    return new_dir_contents[new_dir_i];
}

void MainController::handleGestureMessage(std::unique_ptr<GestureMessage> ge)
{
	//lonk or short
	//the handz
		//keep track of hands and their gestures
	//
	try
	{
	    std::cout << "Here is where I would do something with this: " << pathToIndex(Graphics::getInstance()->currentHighlightedPath()) << std::endl;
	}
	catch (...)
	{
		std::cout << "Couldn't do debug text! MainController line 192" << std::endl;
	}
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
                         , i == new_dir_i
                         , selected.count( new_dir_contents[i].path() ) );
    }

    Graphics::getInstance()->newObjects(std::move(objs));

}

unsigned int MainController::pathToIndex(std::wstring* p)
{
	if (nullptr == p)
	{
        return -1;
	}
    for ( unsigned int i = 0; i < new_dir_contents.size(); i++)
	{
        if (*p == new_dir_contents[i].path().filename().wstring())
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
