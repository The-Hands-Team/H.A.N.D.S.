#include "MainController.hpp"
#include <iostream>
#include <errno.h>

#include "Graphics/Graphics.hpp"
#include "GestureCapture/GestureCapture.hpp"

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

int main()
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
	updateDirectory(cur_path);
	new_dir_i = 0;

    while (true)
    {
        sendCurrentPath();
        //got something in queue
        Message* ev = event_q->pop();
        processEvent(ev);
        while (event_q->size() != 0)
        {
            ev = event_q->pop();
            processEvent(ev);
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

    if (GESTURE == m->getType())
    {
        GestureMessage* ge = dynamic_cast<GestureMessage*>(m);
        std::cout << ge->getGesture() << " " << ge->getHand() << " " << ge->getDir() << std::endl;
        switch (ge->getGesture())
        {
        case PINCH:
            if( HAND_RIGHT == ge->getHand() )
            {
                select();
                break;
            }
            else
            {
                copyInto( cur_path );
            }
        case SCREEN_TAP:
            if( HAND_RIGHT == ge->getHand() )
            {
                std::cout << cur_path << std::endl;
            }
            break;
        case CIRCLE:
            break;
        case SWIPE:
            if( HAND_RIGHT == ge->getHand() )
            {
                switch (ge->getDir())
                {
                case UP:
                    chdirUp();
                    break;
                case DOWN:
                    chdirDown();
                    break;
                case RIGHT:
                    iterateForward();
                    break;
                case LEFT:
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
                case UP:
                    copyInto(cur_path.parent_path());
                    break;
                case DOWN:
                    copyInto(curEntry());
                    break;
                case RIGHT:
                    moveInto(cur_path.parent_path());
                    break;
                case LEFT:
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
    else if(KEYPRESS == m->getType())
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
    else if(FILESYSTEM == m->getType())
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
                FileManager::getInstance()->replyToError( fe->get_t_id(), FileManager::IGNORE );
                updateDirectory(cur_path);
                break;
            default:
                std::cerr << fe->getErrCode().value() << fe->getErrCode().message() << " " << fe->getPath1() << " " << fe->getPath2() << std::endl;
                FileManager::getInstance()->replyToError( fe->get_t_id(), FileManager::TERMINATE );
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
    size_t length = new_dir_contents.size();
    dirObject* objs = nullptr;

    if( length > 0 ) objs = new dirObject[length];

	for (unsigned int i = 0; i < length; i++)
	{
        const std::wstring name = new_dir_contents[i].path().filename().wstring();
        objs[i] = dirObject(fs::is_directory(new_dir_contents[i].path())?'d':'f',0.25f*(i/5),0.25f*(i%5),name, i == new_dir_i, selected.count(new_dir_contents[i].path()));
	}

    newObjects(objs, length);

}
