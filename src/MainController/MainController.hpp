#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <vector>
#include <unordered_set>
#include <thread>
#include "GestureQueue.hpp"
#include "FileSystem/FileManager.hpp"

class MainController {
private:
    static MainController* curInstance;
    GestureQueue gq;
    GestureCapture gc;
    FileManager fm;
    std::thread graphics_t;
    void sendCurrentPath();
    bool ignore_new;
    void processEvent(std::unique_ptr<Message>&);
	void handleGestureMessage(std::unique_ptr<GestureMessage>);

    void chdirUp();
    void chdirDown();
    void iterateForward();
    void iterateBack();
    void select();
    void copyInto(fs::path);
    void moveInto(fs::path);
    void deleteSelected();
    void clearSelected();
    void clearClipboard();
    void pasteFromClipboard();
    fs::directory_entry curEntry();

    unsigned int new_dir_i;
    std::vector<fs::directory_entry> new_dir_contents;
    std::unordered_set<fs::path> selected;
    void updateDirectory(fs::path);
    fs::path cur_path;
    fs::directory_iterator dir_it;
    unsigned int pathToIndex(std::wstring);

    enum GestType curGests[2]; 
    const int LEFT = 0;
    const int RIGHT = 1;   
    bool isLong(enum GestType);
    bool hasTarget();

    std::wstring heldPath;

public:
    MainController();
    ~MainController();
    static MainController* getInstance();
    void mainLoop();
};

#endif
