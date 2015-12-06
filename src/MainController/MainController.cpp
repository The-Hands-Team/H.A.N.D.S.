#include "MainController.hpp"
#include <iostream>

MainController* MainController::curInstance = nullptr;

MainController::MainController()
{
	curInstance = this;
	event_q = new GestureQueue();
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

void MainController::initThread()
{
	getInstance()->mainLoop();
}

void MainController::mainLoop()
{
	cur_path = fs::path("tests/test_dir");
	while (true)
	{
		std::unique_lock<std::mutex> lk(event_m);
		event_cv.wait(lk, [&]{return event_q->size() != 0;});
		//got something in queue
		GestureEvent* ev = event_q->pop();
		//TODO: ACTUALLY PROCESS THE EVENT
		processEvent(ev);
		while (event_q->size() != 0)
		{
			ev = event_q->pop();
			processEvent(ev);
		}
	}	
}

void MainController::pushEvent(GestureEvent* ge)
{
	if (!ignore_new)
	{
		event_q->push(ge);
		event_cv.notify_one();
	}
}

void MainController::processEvent(GestureEvent* ge)
{
	std::cout << "This is the part where an event gets processed! It was: " << ge->getName() << std::endl;
	switch (ge->getGestureType()) {
		case CIRCLE:
			std::cout << "Circle!" << std::endl;
			break;
		case KEY_TAP:
			std::cout << "Key Tap!" << std::endl;
			for(auto& p: fs::directory_iterator(cur_path))
				std::cout << p << std::endl;
			break;
		case SCREEN_TAP:
			std::cout << "Screen Tap!" << std::endl;
			break;
		case SWIPE:
			std::cout << "Swipe!" << std::endl;
			break;
		default:
			std::cout<<"Gesture" << ge->getName() << " not supported\n";
			break;
	}
}
