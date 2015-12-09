OS := $(shell uname)
ARCH := $(shell uname -m)


C_FLAGS := -std=c++11 -Wall -Wextra -g -isysteminclude/ -Isrc/

STD_LIB := -lpthread -lstdc++fs
IRR_LIB := -Llib/irrlicht/ -lIrrlicht -lGL -lXxf86vm -lXext -lX11 -lXcursor
MY_LIBS := lib/FileManager.o lib/GestureCapture.o lib/GestureEvent.o lib/GestureQueue.o lib/MainController.o lib/Graphics.o

ifeq ($(OS), Linux)
  ifeq ($(ARCH), x86_64)
    LEAP_LIBRARY := lib/LeapMotion/x64/libLeap.so -Wl,-rpath,lib/LeapMotion/x64
  else
    LEAP_LIBRARY := lib/LeapMotion/x86/libLeap.so -Wl,-rpath,lib/LeapMotion/x86
  endif
else
  # OS X
  LEAP_LIBRARY := lib/LeapMotion/libLeap.dylib
endif

all: $(MY_LIBS)
	$(CXX) $(C_FLAGS) $(MY_LIBS) $(LEAP_LIBRARY) $(IRR_LIB) $(STD_LIB) -o Run

ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib lib/LeapMotion/libLeap.dylib GestureTest
endif

lib/FileManager.o: src/FileSystem/FileManager.cpp
	$(CXX) $(C_FLAGS) -c src/FileSystem/FileManager.cpp -o lib/FileManager.o
	
lib/GestureCapture.o: src/GestureCapture/GestureCapture.cpp
	$(CXX) $(C_FLAGS) -c src/GestureCapture/GestureCapture.cpp -o lib/GestureCapture.o
	
lib/GestureEvent.o: src/GestureCapture/GestureEvent.cpp
	$(CXX) $(C_FLAGS) -c src/GestureCapture/GestureEvent.cpp -o lib/GestureEvent.o
	
lib/GestureQueue.o: src/MainController/GestureQueue.cpp
	$(CXX) $(C_FLAGS) -c src/MainController/GestureQueue.cpp -o lib/GestureQueue.o
	
lib/MainController.o: src/MainController/MainController.cpp
	$(CXX) $(C_FLAGS) -c src/MainController/MainController.cpp -o lib/MainController.o
	
lib/Graphics.o: src/Graphics/Graphics.cpp src/Graphics/Graphics.hpp
	$(CXX) $(C_FLAGS) -c src/Graphics/Graphics.cpp -o lib/Graphics.o

clean:
	rm -rf Run $(MY_LIBS)
