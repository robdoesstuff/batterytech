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
NACL_SDK_ROOT?=/Users/rgreen/nacl_sdk/pepper_23
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

#
# Project Settings
#
TOOLCHAIN=newlib

MY_CXXFLAGS:=$(NACL_CXXFLAGS) -I../../src
MY_CXXFLAGS+=-I../../src/ppapi/lib/gl

BT_SOURCES=\
hello_world.cc \
matrix.cc

# TODO - 32 and 64 bit versions of each .o
BT_REL_OBJECTS=$(addprefix newlib/Release\, $(addsuffix .o,$(BT_SOURCES)))
BT_REL_DEPS=$(BT_REL_OBJECTS:.o=.d)
BT_DBG_OBJECTS=$(addprefix newlib/Debug\, $(addsuffix .o,$(BT_SOURCES)))
BT_DBG_DEPS=$(BT_DBG_OBJECTS:.o=.d)

#
# Default target
#
all:

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

NEWLIB_CC?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-gcc -c
NEWLIB_CXX?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-g++ -c -std=gnu++98
NEWLIB_LINK?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-g++ -Wl,-as-needed
NEWLIB_LIB?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-ar r
NEWLIB_DUMP?=$(TC_PATH)/$(OSNAME)_x86_newlib/x86_64-nacl/bin/objdump
NEWLIB_CCFLAGS?=-MMD -pthread $(NACL_WARNINGS) -idirafter $(NACL_SDK_ROOT)/include
NEWLIB_LDFLAGS?=-pthread


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
HELLO_WORLD_GLES_OBJS:=hello_world matrix

#
# Rules for newlib toolchain
#
newlib:
	$(MKDIR) newlib
newlib/$(OUTPUT_DIR): | newlib
	$(MKDIR) newlib/Debug
newlib/Release: | newlib
	$(MKDIR) newlib/Release

# Include header dependency files.
-include newlib/Debug/*.d
-include newlib/Release/*.d

#TODO - get 32 and 64 bit .o dependencies in from object list
#TODO - get rid of newlib dir
#TODO - add subdirs and static modules in - full project build

PPAPI_DEBUG:=$(abspath newlib/Debug/hello_world_gles_<ARCH>.nexe);application/x-ppapi-debug
newlib/Debug/hello_world_x86_32.o : hello_world.cc $(THIS_MAKE) | newlib/Debug
	$(NEWLIB_CC) -o $@ $< $(DEBUG_FLAGS) -m32 $(NEWLIB_CCFLAGS) $(MY_CXXFLAGS)  

newlib/Debug/matrix_x86_32.o : matrix.cc $(THIS_MAKE) | newlib/Debug
	$(NEWLIB_CC) -o $@ $< $(DEBUG_FLAGS) -m32 $(NEWLIB_CCFLAGS) $(MY_CXXFLAGS)  


HELLO_WORLD_GLES_NEWLIB_DEBUG_x86_32_O:=$(patsubst %,newlib/Debug/%_x86_32.o,$(HELLO_WORLD_GLES_OBJS))
newlib/Debug/hello_world_gles_x86_32.nexe : $(HELLO_WORLD_GLES_NEWLIB_DEBUG_x86_32_O)
	$(NEWLIB_LINK) -o $@ $^ -g -m32 $(NEWLIB_LDFLAGS) $(HELLO_WORLD_GLES_LDFLAGS) -L$(NACL_SDK_ROOT)/lib/$(OSNAME)_x86_32_newlib/Debug $(LINKED_LIBS)

newlib/Debug/hello_world_x86_64.o : hello_world.cc $(THIS_MAKE) | newlib/Debug
	$(NEWLIB_CC) -o $@ $< $(DEBUG_FLAGS) -m64 $(NEWLIB_CCFLAGS) $(MY_CXXFLAGS)  

newlib/Debug/matrix_x86_64.o : matrix.cc $(THIS_MAKE) | newlib/Debug
	$(NEWLIB_CC) -o $@ $< $(DEBUG_FLAGS) -m64 $(NEWLIB_CCFLAGS) $(MY_CXXFLAGS)  


HELLO_WORLD_GLES_NEWLIB_DEBUG_x86_64_O:=$(patsubst %,newlib/Debug/%_x86_64.o,$(HELLO_WORLD_GLES_OBJS))
newlib/Debug/hello_world_gles_x86_64.nexe : $(HELLO_WORLD_GLES_NEWLIB_DEBUG_x86_64_O)
	$(NEWLIB_LINK) -o $@ $^ -g -m64 $(NEWLIB_LDFLAGS) $(HELLO_WORLD_GLES_LDFLAGS) -L$(NACL_SDK_ROOT)/lib/$(OSNAME)_x86_64_newlib/Debug $(LINKED_LIBS)


ALL_TARGETS+=newlib/Debug/hello_world_gles.nmf
newlib/Debug/hello_world_gles.nmf : newlib/Debug/hello_world_gles_x86_32.nexe newlib/Debug/hello_world_gles_x86_64.nexe
	$(NMF) -D $(NEWLIB_DUMP) -o $@ $^ -t newlib -s newlib/Debug

PPAPI_RELEASE:=$(abspath newlib/Release/hello_world_gles_x86_64.nexe);application/x-ppapi-release
newlib/Release/hello_world_x86_32.o : hello_world.cc $(THIS_MAKE) | newlib/Release
	$(NEWLIB_CC) -o $@ $< $(RELEASE_FLAGS) -m32 $(NEWLIB_CCFLAGS) $(MY_CXXFLAGS)  

newlib/Release/matrix_x86_32.o : matrix.cc $(THIS_MAKE) | newlib/Release
	$(NEWLIB_CC) -o $@ $< $(RELEASE_FLAGS) -m32 $(NEWLIB_CCFLAGS) $(MY_CXXFLAGS)  


HELLO_WORLD_GLES_NEWLIB_RELEASE_x86_32_O:=$(patsubst %,newlib/Release/%_x86_32.o,$(HELLO_WORLD_GLES_OBJS))
newlib/Release/hello_world_gles_x86_32.nexe : $(HELLO_WORLD_GLES_NEWLIB_RELEASE_x86_32_O)
	$(NEWLIB_LINK) -o $@ $^ -m32 $(NEWLIB_LDFLAGS) $(HELLO_WORLD_GLES_LDFLAGS) -L$(NACL_SDK_ROOT)/lib/$(OSNAME)_x86_32_newlib/Release $(LINKED_LIBS)

newlib/Release/hello_world_x86_64.o : hello_world.cc $(THIS_MAKE) | newlib/Release
	$(NEWLIB_CC) -o $@ $< $(RELEASE_FLAGS) -m64 $(NEWLIB_CCFLAGS) $(MY_CXXFLAGS)  

newlib/Release/matrix_x86_64.o : matrix.cc $(THIS_MAKE) | newlib/Release
	$(NEWLIB_CC) -o $@ $< $(RELEASE_FLAGS) -m64 $(NEWLIB_CCFLAGS) $(MY_CXXFLAGS)  


HELLO_WORLD_GLES_NEWLIB_RELEASE_x86_64_O:=$(patsubst %,newlib/Release/%_x86_64.o,$(HELLO_WORLD_GLES_OBJS))
newlib/Release/hello_world_gles_x86_64.nexe : $(HELLO_WORLD_GLES_NEWLIB_RELEASE_x86_64_O)
	$(NEWLIB_LINK) -o $@ $^ -m64 $(NEWLIB_LDFLAGS) $(HELLO_WORLD_GLES_LDFLAGS) -L$(NACL_SDK_ROOT)/lib/$(OSNAME)_x86_64_newlib/Release $(LINKED_LIBS)


ALL_TARGETS+=newlib/Release/hello_world_gles.nmf
newlib/Release/hello_world_gles.nmf : newlib/Release/hello_world_gles_x86_32.nexe newlib/Release/hello_world_gles_x86_64.nexe
	$(NMF) -D $(NEWLIB_DUMP) -o $@ $^ -t newlib -s newlib/Release


#
# Target to remove temporary files
#
.PHONY: clean
clean:
	$(RM) -fr newlib/Debug
	$(RM) -fr newlib/Release

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


