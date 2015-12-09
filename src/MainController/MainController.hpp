#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include "GestureQueue.hpp"
#include "Graphics/Graphics.hpp"
#include <mutex>
#include <cstring>
#include <condition_variable>
#include "FileSystem/FileManager.hpp"

class MainController {
private:
	static MainController* curInstance;
	MainController();
	~MainController();
    void sendCurrentPath();
	bool ignore_new;
	void processEvent(Message*);

	fs::path cur_path;
	fs::directory_iterator dir_it;
public:
	static MainController* getInstance();
	static void initThread();
    void mainLoop();
};

#endif
