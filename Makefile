#############################################################################
#
# Makefile for RF24SN C++ Server
#
# License: MIT
# Author:  Vaclav Synacek
# Date:    2014 (version 1.0)
#
# Description:
# ------------
# use make all and make install to install
# You can change the install directory by editing the prefix line
# Requires librf24 installed
#


# The recommended compiler flags for the Raspberry Pi
CC_FLAGS=-Wall -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
LD_FLAGS=-lrf24 -lmosquitto
CPP_FILES=$(wildcard *.cpp)
OBJ_FILES=$(CPP_FILES:.cpp=.o)

%.o: %.cpp
	g++ $(CC_FLAGS) -c -o $@ $<
   
all: $(OBJ_FILES)
	g++ $(LD_FLAGS) -o RF24SN $^

CC_FLAGS += -MMD
-include $(OBJFILES:.o=.d)