#include "MainController.hpp"

MainController* MainController::curInstance = nullptr;

MainController::MainController()
{
	curInstance = this;
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
