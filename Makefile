OS := $(shell uname)
ARCH := $(shell uname -m)
LIBRARIES := -lpthread -lstdc++fs

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
	$(CXX) -std=c++11 -Wall -Wextra -g -Iinclude/ lib/*.o -o run $(LEAP_LIBRARY) $(LIBRARIES)
ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib lib/LeapMotion/libLeap.dylib GestureTest
endif

clean:
	rm -rf run lib/*.o
