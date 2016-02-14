#include "MainController.hpp"
#include <iostream>

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
    ///**/dir_it = fs::directory_iterator(cur_path);
	/**/updateDirectory(cur_path);
	new_dir_i = 0;

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
		//TODO: update graphiczz
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


void MainController::copyCurrent()
{
	std::vector<fs::path> source;
	std::vector<fs::path> dest;
	
	for( auto const v : selected )
	{
		source.emplace_back( v );
		dest.emplace_back( cur_path / v.filename() / "_copy" );
	}
	
    FileManager::getInstance()->copyFiles(source,dest);
    
	clearSelected();
}
void MainController::copyInto()
{
	fs::path dest_p = curEntry().path();
	std::vector<fs::path> source;
	std::vector<fs::path> dest;
	if (fs::is_directory(dest_p))
	{
		for( auto const v : selected )
		{
			source.emplace_back( v );
			dest.emplace_back( dest_p / v.filename() );
		}
		
		FileManager::getInstance()->copyFiles(source,dest);
		
		clearSelected();
	}
}
void MainController::copyUp()
{
	fs::path dest_p = cur_path.parent_path();
	std::vector<fs::path> source;
	std::vector<fs::path> dest;
	
	if (fs::is_directory(dest_p))
	{
		for( auto const v : selected )
		{
			source.emplace_back( v );
			dest.emplace_back( dest_p / v.filename() );
		}
		
		FileManager::getInstance()->copyFiles(source,dest);
		
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
    //std::cout << "This is the part where an event gets processed! It was: " << ge->getName() << std::endl;
    //std::cout << m->getType() << std::endl;

    if (GESTURE == m->getType())
    {
        GestureMessage* ge = dynamic_cast<GestureMessage*>(m);
        //std::cout << ge->getGesture() << std::endl;
        switch (ge->getGesture())
        {
        case KEY_TAP:
        {
			/**/copyCurrent();
            //break;
            ///**/fs::path dest = dir_it->path();
            ///**/dest += fs::path("_copy");
            ///**/FileManager::getInstance()->copyFile(dir_it->path(),dest);
            ///**/std::this_thread::sleep_for(std::chrono::milliseconds(500));
            ///**/sendCurrentPath();
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
				/**/chdirUp();
                ///**/if (cur_path.parent_path() != "")
                ///**/{
                   // /**/fs::path parent = cur_path.parent_path();
                    ///**/dir_it = fs::directory_iterator(parent);
                    ///**/while (dir_it != fs::end(dir_it)
                          //  && dir_it->path() != cur_path)
                        ///**/dir_it++;
                    ///**/cur_path = parent;
                    ///**/sendCurrentPath();
                ///**/}
                break;
            case DOWN:
				/**/chdirDown();
                ///**/if (fs::is_directory(dir_it->path()))
                ///**/{
                    ///**/cur_path = dir_it->path();
                    ///**/dir_it = fs::directory_iterator(cur_path);
                    ///**/sendCurrentPath();
                ///**/}
                std::cout << cur_path << std::endl;
                break;
            case RIGHT:
				/**/iterateForward();
                ///**/dir_it++;
                ///**/if (dir_it == fs::end(dir_it))
                ///**/{
                   // /**/dir_it = fs::directory_iterator(cur_path);
                ///**/}
                ///**/std::cout << dir_it->path() << std::endl;
                /**/std::cout << curEntry().path() << std::endl;
                break;
			case LEFT:
				/**/iterateBack();
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
					copyCurrent();
                    break;
                }
            case irr::EKEY_CODE::KEY_KEY_S:
                {
					select();
                    break;
                }
            case irr::EKEY_CODE::KEY_KEY_D:
                {
                    break;
                }
            case irr::EKEY_CODE::KEY_UP:
				chdirUp();
                break;
            case irr::EKEY_CODE::KEY_DOWN:
				chdirDown();
                std::cout << cur_path << std::endl;
                break;
            case irr::EKEY_CODE::KEY_RIGHT:
				iterateForward();
                std::cout << curEntry().path() << std::endl;
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
                sendCurrentPath();
                break;
            default:
                std::cerr << fe->getErrCode().message() << std::endl;
                FileManager::getInstance()->replyToError( fe->get_t_id(), FileManager::TERMINATE );
                sendCurrentPath();
                break;
                //we don't know;
        }
    }
    delete m;
}

void MainController::sendCurrentPath()
{
    ///**/size_t length = 0;
    // TODO Also keep a copy of the list for ourselves, possibly with more information
    ///**/for( fs::directory_iterator it (cur_path); it != fs::end(it); it++) length++;
    ///**/dirObject* objs = new dirObject[length];
    size_t length = new_dir_contents.size();
    dirObject* objs = nullptr;
    
    if( length > 0 ) objs = new dirObject[length];

	for (unsigned int i = 0; i < length; i++)
	{
        const std::wstring name = new_dir_contents[i].path().filename().wstring();
        objs[i] = dirObject(fs::is_directory(new_dir_contents[i].path())?'d':'f',0.25f*(i/5),0.25f*(i%5),name, i == new_dir_i, selected.count(new_dir_contents[i].path()));
	}
    /*for( fs::directory_iterator it (cur_path); i<length; it++, i++)
    {
        const std::wstring name = it->path().filename().wstring();
        objs[i] = dirObject(fs::is_directory(it->path())?'d':'f',0.25f*(i/5),0.25f*(i%5),name.data(), *it == *dir_it);
    }*/

    ///**/newObjects(objs, length);
    newObjects(objs, length);

}
