#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include "GestureQueue.hpp"
#include <mutex>
#include <condition_variable>
#include "FileSystem/FileManager.hpp"

class MainController {
private:
	static MainController* curInstance;
	MainController();
	~MainController();
	std::mutex event_m;
	std::condition_variable event_cv;
	GestureQueue* event_q;
	bool ignore_new;
	void processEvent(GestureEvent*);
	fs::path cur_path;
	fs::directory_iterator dir_it;
public:
	static MainController* getInstance();
	static void initThread();
	void pushEvent(GestureEvent*);
    void mainLoop();
};

#endif
