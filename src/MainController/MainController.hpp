#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <vector>
#include <unordered_set>
#include "GestureQueue.hpp"
#include "FileSystem/FileManager.hpp"

class MainController {
private:
	static MainController* curInstance;
	MainController();
	~MainController();
    void sendCurrentPath();
	bool ignore_new;
	void processEvent(Message*);

	void chdirUp();
	void chdirDown();
	void iterateForward();
	void iterateBack();
    void select();
	void copyCurrent();
    void copyInto();
    void copyUp();
    void clearSelected();
	fs::directory_entry curEntry();

	unsigned int new_dir_i;
	std::vector<fs::directory_entry> new_dir_contents;
    std::unordered_set<fs::path> selected;
	void updateDirectory(fs::path);

	fs::path cur_path;
	fs::directory_iterator dir_it;
public:
	static MainController* getInstance();
	static void initThread();
    void mainLoop();
};

#endif
