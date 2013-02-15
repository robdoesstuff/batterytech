#
# GNU Make based build file.  For details on GNU Make see:
#   http://www.gnu.org/software/make/manual/make.html
#

#
# Get pepper directory for toolchain and includes.
#
# If NACL_SDK_ROOT is not set, look in home/nacl_sdk
#
THIS_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
NACL_SDK_ROOT?=/nacl_sdk/pepper_23
CHROME_PATH?=Undefined

#
# Defaults
#
NACL_WARNINGS:=-Wno-long-long -Wall -Wswitch-enum -Werror -pedantic

RELEASE_DIR=Release
DEBUG_DIR=Debug

RELEASE_FLAGS=-O2 -DCHROME -DBATTERYTECH_INCLUDE_ASSIMP -DBATTERYTECH_INCLUDE_BOX2D
DEBUG_FLAGS=-g -O0 -DCHROME -DBATTERYTECH_INCLUDE_ASSIMP -DBATTERYTECH_INCLUDE_BOX2D
LINKED_LIBS=-lppapi_gles2 -lppapi -lpthread

ifeq (${DEBUG},true)
	BUILD_FLAGS=$(DEBUG_FLAGS)
	OUTPUT_DIR=$(DEBUG_DIR)
	BUILD_DIR=$(DEBUG_DIR)
else
	BUILD_FLAGS=$(RELEASE_FLAGS)
	OUTPUT_DIR=$(RELEASE_DIR)
	BUILD_DIR=$(RELEASE_DIR)
endif
#
# Project Settings
#
TOOLCHAIN=newlib

MY_CXXFLAGS:=$(NACL_CXXFLAGS) -I../../src
MY_CXXFLAGS+=-I../../src/ppapi/lib/gl

APP_SOURCE_DIR=../app-src
SOURCE_DIR=./

BT_SOURCES=\
hello_world.cc \
matrix.cc

BT_OBJECTS_32=$(addprefix $(BUILD_DIR)\, $(addsuffix _x86_32.o,$(BT_SOURCES)))
BT_OBJECTS_64=$(addprefix $(BUILD_DIR)\, $(addsuffix _x86_64.o,$(BT_SOURCES)))
BT_DEPS=$(BT_OBJECTS:.o=.d)

#
# Alias for standard commands
#
CP:=python $(NACL_SDK_ROOT)/tools/oshelpers.py cp
MKDIR:=python $(NACL_SDK_ROOT)/tools/oshelpers.py mkdir
MV:=python $(NACL_SDK_ROOT)/tools/oshelpers.py mv
RM:=python $(NACL_SDK_ROOT)/tools/oshelpers.py rm

#
# Compute path to requested NaCl Toolchain
#
OSNAME:=$(shell python $(NACL_SDK_ROOT)/tools/getos.py)
TC_PATH:=$(abspath $(NACL_SDK_ROOT)/toolchain)


#
# Verify we have a valid NACL_SDK_ROOT by looking for the toolchain directory
#
ifeq (,$(wildcard $(TC_PATH)))
$(warning No valid NACL_SDK_ROOT at $(NACL_SDK_ROOT))
ifeq ($(origin NACL_SDK_ROOT), 'file')
$(error Override the default value via enviornment variable, or command-line.)
else
$(error Fix the NACL_SDK_ROOT specified in the environment or command-line.)
endif
endif


#
# Disable DOS PATH warning when using Cygwin based NaCl tools on Windows
#
CYGWIN ?= nodosfilewarning
export CYGWIN


#
# Defaults for TOOLS
#

CC=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-gcc -c
CXX=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-g++ -c -std=gnu++98
LINK=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-g++ -Wl,-as-needed
LIB=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-ar r
DUMP=$(TC_PATH)/$(OSNAME)_x86_x86_64-nacl/bin/objdump
CCFLAGS=-MMD -pthread $(NACL_WARNINGS) -idirafter $(NACL_SDK_ROOT)/include
LDFLAGS=-pthread


#
# NMF Manifiest generation
#
# Use the python script create_nmf to scan the binaries for dependencies using
# objdump.  Pass in the (-L) paths to the default library toolchains so that we
# can find those libraries and have it automatically copy the files (-s) to
# the target directory for us.
NMF:=python $(NACL_SDK_ROOT)/tools/create_nmf.py


#
# Verify we can find the Chrome executable if we need to launch it.
#
.PHONY: CHECK_FOR_CHROME
CHECK_FOR_CHROME:
ifeq (,$(wildcard $(CHROME_PATH)))
	$(warning No valid Chrome found at CHROME_PATH=$(CHROME_PATH))
	$(error Set CHROME_PATH via an environment variable, or command-line.)
else
	$(warning Using chrome at: $(CHROME_PATH))
endif

#
# Per target object lists
#
BATTERYTECH_OBJS:=hello_world.cc matrix.cc


# Include header dependency files.
-include $(BUILD_DIR)/*.d

#TODO - add subdirs and static modules in - full project build

PPAPI:=$(abspath $(OUTPUT_DIR)/hello_world_gles_<ARCH>.nexe);application/x-ppapi-debug
BATTERYTECH_x86_32_O:=$(patsubst %,$(BUILD_DIR)/%.x86_32_o,$(BATTERYTECH_OBJS))
BATTERYTECH_x86_64_O:=$(patsubst %,$(BUILD_DIR)/%.x86_64_o,$(BATTERYTECH_OBJS))

#
# Default target
#
all:| $(OUTPUT_DIR)
	
$(OUTPUT_DIR):
	$(MKDIR) $(OUTPUT_DIR)
	
# build 32 bit objects

$(BATTERYTECH_x86_32_O): $(BUILD_DIR)/%.x86_32_o : %
	$(CC) -o $@ $< $(BUILD_FLAGS) -m32 $(CCFLAGS) $(MY_CXXFLAGS)

# link 32 bit exe

$(OUTPUT_DIR)/hello_world_gles_x86_32.nexe : $(BATTERYTECH_x86_32_O)
	$(LINK) -o $@ $^ -g -m32 $(LDFLAGS) $(HELLO_WORLD_GLES_LDFLAGS) -L$(NACL_SDK_ROOT)/lib/$(OSNAME)_x86_32_$(BUILD_DIR) $(LINKED_LIBS)

# build 64 bit debug objects

$(BATTERYTECH_x86_64_O): $(BUILD_DIR)/%.x86_64_o : %
	$(CC) -o $@ $< $(BUILD_FLAGS) -m64 $(CCFLAGS) $(MY_CXXFLAGS)

# link 64 bit exe

$(OUTPUT_DIR)/hello_world_gles_x86_64.nexe : $(BATTERYTECH_x86_64_O)
	$(LINK) -o $@ $^ -g -m64 $(LDFLAGS) $(HELLO_WORLD_GLES_LDFLAGS) -L$(NACL_SDK_ROOT)/lib/$(OSNAME)_x86_64_$(BUILD_DIR) $(LINKED_LIBS)

# build debug manifest

ALL_TARGETS+=$(OUTPUT_DIR)/hello_world_gles.nmf
$(OUTPUT_DIR)/hello_world_gles.nmf : $(OUTPUT_DIR)/hello_world_gles_x86_32.nexe $(OUTPUT_DIR)/hello_world_gles_x86_64.nexe
	$(NMF) -D $(DUMP) -o $@ $^ -t newlib -s $(OUTPUT_DIR)


#
# Target to remove temporary files
#
.PHONY: clean
clean:
	$(RM) -fr Debug
	$(RM) -fr Release

all: $(ALL_TARGETS)




RUN: all
	python ../httpd.py

CONFIG?=Debug
PAGE?=index_$(TOOLCHAIN)_$(CONFIG).html

LAUNCH: CHECK_FOR_CHROME all
ifeq (,$(wildcard $(PAGE)))
	$(warning No valid HTML page found at $(PAGE))
	$(error Make sure TOOLCHAIN and CONFIG are properly set)
endif
	$(CHROME_PATH) $(NEXE_ARGS) --register-pepper-plugins="$(PPAPI_DEBUG),$(PPAPI_RELEASE)" localhost:5103/$(PAGE)


