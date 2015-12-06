OS := $(shell uname)
ARCH := $(shell uname -m)
LIBRARIES := -lpthread -lstdc++fs -Llib/irrlicht/ -lIrrlicht -L/usr/X11R6/lib64 -lGL -lXxf86vm -lXext -lX11 -lXcursor 

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

all:
	make -C src/MainController
	make -C src/LeapMotion
	make -C src/FileSystem
	make -C src/Graphics
	$(CXX) -std=c++11 -Wall -Wextra -g -Iinclude/ lib/FileSystem.o lib/GestureCapture.o lib/GestureEvent.o lib/GestureQueue.o lib/MainController.o lib/irrlicht.o -o run $(LEAP_LIBRARY) $(LIBRARIES)

ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib lib/LeapMotion/libLeap.dylib GestureTest
endif

clean:
	rm -rf run lib/*.o
