#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

class MainController {
private:
	MainController();
	~MainController();
	static MainController* curInstance;

public:
	static MainController* getInstance();
};

#endif
