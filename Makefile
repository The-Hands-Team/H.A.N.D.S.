OS := $(shell uname)
ARCH := $(shell uname -m)


C_FLAGS = -std=c++11 -Wall -Wextra -Wpedantic -g -D DEBUG=$(DEBUG) -isysteminclude/ -Isrc/
FOLDER := lib/Debug/
DEBUG := 1
TEMPFILE := temp.mk

STD_LIB := -lpthread -lstdc++fs
IRR_LIB = -Llib/irrlicht/ -lIrrlicht -lGL -lXxf86vm -lXext -lX11 -lXcursor
MY_LIBS = $(FOLDER)FileManager.o $(FOLDER)GestureCapture.o $(FOLDER)MainController.o $(FOLDER)Graphics.o $(FOLDER)GestureQueue.o
Files = make/FileManager.d make/GestureCapture.d make/MainController.d make/Graphics.d make/GestureQueue.d
#$(FOLDER)GestureEvent.o
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

Debug: $(MY_LIBS)
	$(CXX) $(C_FLAGS) $(MY_LIBS) $(LEAP_LIBRARY) $(IRR_LIB) $(STD_LIB) -o Run; \
	rm $(TEMPFILE)

Release: setRelease $(MY_LIBS)
	$(CXX) $(C_FLAGS) $(MY_LIBS) $(LEAP_LIBRARY) $(IRR_LIB) $(STD_LIB) -o Run; \
	rm $(TEMPFILE)

setRelease:
	$(eval FOLDER=lib/Release/)
	$(eval DEBUG=0)

ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib $(FOLDER)LeapMotion/libLeap.dylib GestureTest
endif

-include $(TEMPFILE)

$(TEMPFILE):
	touch $(TEMPFILE)
	$(CXX) $(C_FLAGS) -MM -MT $(FOLDER)MainController.o src/MainController/MainController.cpp >> $(TEMPFILE)
	printf "	$(CXX) $(C_FLAGS) -c $$< -o $(FOLDER)MainController.o\n\n" >> $(TEMPFILE)
	$(CXX) $(C_FLAGS) -MM -MT $(FOLDER)FileManager.o src/FileSystem/FileManager.cpp >> $(TEMPFILE)
	printf "	$(CXX) $(C_FLAGS) -c $$< -o $(FOLDER)FileManager.o\n\n" >> $(TEMPFILE)
	$(CXX) $(C_FLAGS) -MM -MT $(FOLDER)GestureCapture.o src/GestureCapture/GestureCapture.cpp >> $(TEMPFILE)
	printf "	$(CXX) $(C_FLAGS) -c $$< -o $(FOLDER)GestureCapture.o\n\n" >> $(TEMPFILE)
	$(CXX) $(C_FLAGS) -MM -MT $(FOLDER)GestureQueue.o src/MainController/GestureQueue.cpp >> $(TEMPFILE)
	printf "	$(CXX) $(C_FLAGS) -c $$< -o $(FOLDER)GestureQueue.o\n\n" >> $(TEMPFILE)
	$(CXX) $(C_FLAGS) -MM -MT $(FOLDER)Graphics.o src/Graphics/Graphics.cpp >> $(TEMPFILE)
	printf "	$(CXX) $(C_FLAGS) -c $$< -o $(FOLDER)Graphics.o\n\n" >> $(TEMPFILE)


clean:
	rm -rf Run $(MY_LIBS); \
	rm $(TEMPFILE)

releaseClean: setRelease clean
	

.SILENT: $(TEMPFILE)
