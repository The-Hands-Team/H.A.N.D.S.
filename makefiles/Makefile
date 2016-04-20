# Makefile
#
# This makefile searches for .cpp files and creates a temporary
# makefile that contains the rules for making the corresponding
# object files. It then includes that makefile and builds those
# object files. Finally it links everything into an executable.
#

########## VARIABLES ##########
OS := $(shell uname)
ARCH := $(shell uname -m)

# Finds all .cpp files in src/
FILES := $(shell find src/ -name *.cpp -printf "%p ")
# Every .cpp file has a corresponding .o file
MY_LIBS = $(addprefix lib/_output/,$(addsuffix .o,$(basename $(notdir $(FILES)))))
# Compiler flags
CXXFLAGS = -std=c++14 -Wall -Wextra -Wpedantic -g -isysteminclude/ -Isrc/
# The temporary makefile to use
TEMPFILE := makefiles/temp.mk

### Libraries we include
STD_LIB := -lpthread -lstdc++fs
IRR_LIB := -Llib/irrlicht/ -lIrrlicht -lGL -lXxf86vm -lXext -lX11 -lXcursor
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
ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib lib/_output/LeapMotion/libLeap.dylib GestureTest
endif


########## FUNCTIONS ##########
# Function used to write rules in the temporary makefile
define createStatement =
	$(CXX) $(CXXFLAGS) -MM -MT $(addprefix lib/_output/,$(addsuffix .o,$(basename $(notdir $(1))))) $(1) >> $(TEMPFILE); \
	printf "	$(CXX) $(CXXFLAGS) -c $$< -o \$$@\n\n" >> $(TEMPFILE);
endef

########## TARGETS ##########

# Default target
all: $(MY_LIBS) | lib/_output
	$(CXX) $(CXXFLAGS) $(MY_LIBS) $(LEAP_LIBRARY) $(IRR_LIB) $(STD_LIB) -o Run

# Create Target Folder
lib/_output:
	mkdir lib/_output

# This creates the temporary makefile
$(TEMPFILE):
	touch $(TEMPFILE)
	printf "# Automatically Generated File \n\n" >> $(TEMPFILE)
	$(foreach file, $(FILES), $(call createStatement,$(file)))


### Special targets
.SILENT: $(TEMPFILE)

.PHONY: all

########## INCLUDES ##########
-include $(TEMPFILE)
