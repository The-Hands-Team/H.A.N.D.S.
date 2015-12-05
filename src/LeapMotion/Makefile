OS := $(shell uname)
ARCH := $(shell uname -m)

ifeq ($(OS), Linux)
  ifeq ($(ARCH), x86_64)
    LEAP_LIBRARY := ../../lib/LeapMotion/x64/libLeap.so -Wl,-rpath,../../lib/LeapMotion/x64
  else
    LEAP_LIBRARY := ../../lib/LeapMotion/x86/libLeap.so -Wl,-rpath,../../lib/LeapMotion/x86
  endif
else
  # OS X
  LEAP_LIBRARY := ../../lib/LeapMotion/libLeap.dylib
endif

GestureTest: GestureCapture.cpp
	$(CXX) -Wall -g -I../../include GestureCapture.cpp -o GestureTest $(LEAP_LIBRARY)
ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib ../../lib/LeapMotion/libLeap.dylib GestureTest
endif

clean:
	rm -rf GestureTest GestureTest.dSYM
