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
	PPAPI:=$(abspath $(OUTPUT_DIR)/batterytech_app_<ARCH>.nexe);application/x-ppapi-debug
else
	BUILD_FLAGS=$(RELEASE_FLAGS)
	OUTPUT_DIR=$(RELEASE_DIR)
	BUILD_DIR=$(RELEASE_DIR)
	PPAPI:=$(abspath $(OUTPUT_DIR)/batterytech_app_<ARCH>.nexe);application/x-ppapi
endif
#
# Project Settings
#
TOOLCHAIN=newlib

MY_CXXFLAGS:=$(NACL_CXXFLAGS) -I../../src
MY_CXXFLAGS+=-I../../src/ppapi/lib/gl

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


#TODO - add subdirs and static modules in - full project build

APP_SOURCE_DIR=../app-src
SOURCE_DIR=./

BT_SOURCES=\
hello_world.cc \
matrix.cc

BT_x86_32_O:=$(patsubst %,$(BUILD_DIR)/%.x86_32_o,$(BT_SOURCES))
BT_x86_64_O:=$(patsubst %,$(BUILD_DIR)/%.x86_64_o,$(BT_SOURCES))
BT_DEPS=$(BT_x86_32_O:.x86_32_o=.d)

LUA_SOURCES=\
	bt-lua/lapi.c \
	bt-lua/lauxlib.c \
	bt-lua/lbaselib.c \
	bt-lua/lcode.c \
	bt-lua/ldblib.c \
	bt-lua/ldebug.c \
	bt-lua/ldo.c \
	bt-lua/ldump.c \
	bt-lua/lfunc.c \
	bt-lua/lgc.c \
	bt-lua/linit.c \
	bt-lua/liolib.c \
	bt-lua/llex.c \
	bt-lua/lmathlib.c \
	bt-lua/lmem.c \
	bt-lua/loadlib.c \
	bt-lua/lobject.c \
	bt-lua/lopcodes.c \
	bt-lua/loslib.c \
	bt-lua/lparser.c \
	bt-lua/lstate.c \
	bt-lua/lstring.c \
	bt-lua/lstrlib.c \
	bt-lua/ltable.c \
	bt-lua/ltablib.c \
	bt-lua/ltm.c \
	bt-lua/lua.c \
	bt-lua/luac.c \
	bt-lua/lundump.c \
	bt-lua/lvm.c \
	bt-lua/lzio.c \
	bt-lua/print.c

LUA_x32_0=$(patsubst %,$(BUILD_DIR)/%.x86_32_o,$(LUA_SOURCES))
LUA_x64_0=$(patsubst %,$(BUILD_DIR)/%.x86_64_o,$(LUA_SOURCES))
LUA_DEPS=$(LUA_x32_0:.x86_32_o=.d)
LUA_OBJDIRS=$(BUILD_DIR)/bt-lua
LUA_LIB=$(BUILD_DIR)/liblua.a


ASSIMP_SOURCES=\
	bt-assimp/code/Assimp.cpp \
	bt-assimp/code/AssimpPCH.cpp \
	bt-assimp/code/BAIFileImporter.cpp \
	bt-assimp/code/BaseImporter.cpp \
	bt-assimp/code/BaseProcess.cpp \
	bt-assimp/code/ColladaLoader.cpp \
	bt-assimp/code/ColladaParser.cpp \
	bt-assimp/code/DefaultIOStream.cpp \
	bt-assimp/code/DefaultIOSystem.cpp \
	bt-assimp/code/DefaultLogger.cpp \
	bt-assimp/code/Importer.cpp \
	bt-assimp/code/MaterialSystem.cpp \
	bt-assimp/code/ObjFileImporter.cpp \
	bt-assimp/code/ObjFileMtlImporter.cpp \
	bt-assimp/code/ObjFileParser.cpp \
	bt-assimp/code/ScenePreprocessor.cpp \
	bt-assimp/code/SkeletonMeshBuilder.cpp \
	bt-assimp/code/SpatialSort.cpp \
	bt-assimp/code/LimitBoneWeightsProcess.cpp \
	bt-assimp/contrib/irrXML/irrXML.cpp \
	bt-assimp/contrib/ConvertUTF/ConvertUTF.c

ASSIMP_x32_0=$(patsubst %,$(BUILD_DIR)/%.x86_32_o,$(ASSIMP_SOURCES))
ASSIMP_x64_0=$(patsubst %,$(BUILD_DIR)/%.x86_64_o,$(ASSIMP_SOURCES))
ASSIMP_DEPS=$(ASSIMP_x32_0:.x86_32_o=.d)
ASSIMP_OBJDIRS=$(BUILD_DIR)/bt-assimp/code \
	$(BUILD_DIR)/bt-assimp/contrib/irrXML \
	$(BUILD_DIR)/bt-assimp/contrib/ConvertUTF
ASSIMP_LIB=$(BUILD_DIR)/libassimp.a

BOX2D_SOURCES=\
	bt-box2d/Collision/b2BroadPhase.cpp \
	bt-box2d/Collision/b2CollideCircle.cpp \
	bt-box2d/Collision/b2CollideEdge.cpp \
	bt-box2d/Collision/b2CollidePolygon.cpp \
	bt-box2d/Collision/b2Collision.cpp \
	bt-box2d/Collision/b2Distance.cpp \
	bt-box2d/Collision/b2DynamicTree.cpp \
	bt-box2d/Collision/b2TimeOfImpact.cpp \
	bt-box2d/Collision/Shapes/b2ChainShape.cpp \
	bt-box2d/Collision/Shapes/b2CircleShape.cpp \
	bt-box2d/Collision/Shapes/b2EdgeShape.cpp \
	bt-box2d/Collision/Shapes/b2PolygonShape.cpp \
	bt-box2d/Common/b2BlockAllocator.cpp \
	bt-box2d/Common/b2Draw.cpp \
	bt-box2d/Common/b2Math.cpp \
	bt-box2d/Common/b2Settings.cpp \
	bt-box2d/Common/b2StackAllocator.cpp \
	bt-box2d/Common/b2Timer.cpp \
	bt-box2d/Dynamics/b2Body.cpp \
	bt-box2d/Dynamics/b2ContactManager.cpp \
	bt-box2d/Dynamics/b2Fixture.cpp \
	bt-box2d/Dynamics/b2Island.cpp \
	bt-box2d/Dynamics/b2World.cpp \
	bt-box2d/Dynamics/b2WorldCallbacks.cpp \
	bt-box2d/Dynamics/Contact/b2ChainAndCircleContact.cpp \
	bt-box2d/Dynamics/Contact/b2ChainAndPolygonContact.cpp \
	bt-box2d/Dynamics/Contact/b2CircleContact.cpp \
	bt-box2d/Dynamics/Contact/b2Contact.cpp \
	bt-box2d/Dynamics/Contact/b2ContactSolver.cpp \
	bt-box2d/Dynamics/Contact/b2EdgeAndCircleContact.cpp \
	bt-box2d/Dynamics/Contact/b2EdgeAndPolygonContact.cpp \
	bt-box2d/Dynamics/Contact/b2PolygonAndCircleContact.cpp \
	bt-box2d/Dynamics/Contact/b2PolygonContact.cpp \
	bt-box2d/Dynamics/Joints/b2DistanceJoint.cpp \
	bt-box2d/Dynamics/Joints/b2FrictionJoint.cpp \
	bt-box2d/Dynamics/Joints/b2GearJoint.cpp \
	bt-box2d/Dynamics/Joints/b2Joint.cpp \
	bt-box2d/Dynamics/Joints/b2MouseJoint.cpp \
	bt-box2d/Dynamics/Joints/b2PrismaticJoint.cpp \
	bt-box2d/Dynamics/Joints/b2PulleyJoint.cpp \
	bt-box2d/Dynamics/Joints/b2RevoluteJoint.cpp \
	bt-box2d/Dynamics/Joints/b2RopeJoint.cpp \
	bt-box2d/Dynamics/Joints/b2WeldJoint.cpp \
	bt-box2d/Dynamics/Joints/b2WheelJoint.cpp
	
BOX2D_x32_0=$(patsubst %,$(BUILD_DIR)/%.x86_32_o,$(BOX2D_SOURCES))
BOX2D_x64_0=$(patsubst %,$(BUILD_DIR)/%.x86_64_o,$(BOX2D_SOURCES))
BOX2D_DEPS=$(BOX2D_x32_0:.x86_32_o=.d)
BOX2D_OBJDIRS=$(BUILD_DIR)/bt-box2d/Collision \
	$(BUILD_DIR)/bt-box2d/Collision/Shapes \
	$(BUILD_DIR)/bt-box2d/Common \
	$(BUILD_DIR)/bt-box2d/Dynamics \
	$(BUILD_DIR)/bt-box2d/Dynamics/Contacts \
	$(BUILD_DIR)/bt-box2d/Dynamics/Joints \
	$(BUILD_DIR)/bt-box2d/Rope
BOX2D_LIB=$(BUILD_DIR)/libbox2d.a



# Include header dependency files.
-include $(BT_DEPS)
-include $(LUA_DEPS)
-include $(ASSIMP_DEPS)
-include $(BOX2D_DEPS)


#
# Default target
#
all: | $(OUTPUT_DIR)
	
$(OUTPUT_DIR):
	$(MKDIR) $(OUTPUT_DIR)
	
	
# TODO - fix all these libraries to use the new build stuff.
# TODO - now bring BT lib and App src in and get all building/linking together

# Box2D

box2d: $(BOX2D_LIB)

$(BOX2D_LIB): $(BOX2D_OBJECTS)
	$(STATIC_LINKER) $(ARFLAGS) $@ $(BOX2D_OBJECTS)

$(BOX2D_OBJECTS): $(BUILD_DIR)\\%.o: $(SOURCE_DIR)\\%
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) $< -o $@
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) -MM -MF$(@:.o=.d) -MT$@ $<
	
$(BOX2D_OBJECTS): | $(BOX2D_OBJDIRS)

$(BOX2D_OBJDIRS):
	-md $(BOX2D_OBJDIRS)

# Lua
lua: $(LUA_LIB)

$(LUA_LIB): $(LUA_OBJECTS)
	$(STATIC_LINKER) $(ARFLAGS) $@ $(LUA_OBJECTS)

$(LUA_OBJECTS): $(BUILD_DIR)\\%.o: $(SOURCE_DIR)\\%
	$(CC) $(BUILD_FLAGS) $(CFLAGS) $< -o $@
	$(CC) $(BUILD_FLAGS) $(CFLAGS) -MM -MF$(@:.o=.d) -MT$@ $<

$(LUA_OBJECTS): | $(LUA_OBJDIRS)

$(LUA_OBJDIRS):
	-md $(LUA_OBJDIRS)
	
# Open Asset Importer Library
assimp: $(ASSIMP_LIB)

$(ASSIMP_LIB): $(ASSIMP_OBJECTS)
	$(STATIC_LINKER) $(ARFLAGS) $@ $(ASSIMP_OBJECTS)

$(ASSIMP_OBJECTS): $(BUILD_DIR)\\%.o: $(SOURCE_DIR)\\%
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) $< -o $@
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) -MM -MF$(@:.o=.d) -MT$@ $<
	
$(ASSIMP_OBJECTS): | $(ASSIMP_OBJDIRS)

$(ASSIMP_OBJDIRS):
	-md $(ASSIMP_OBJDIRS)
	
	
# build 32 bit objects

$(BT_x86_32_O): $(BUILD_DIR)/%.x86_32_o : %
	$(CC) -o $@ $< $(BUILD_FLAGS) -m32 $(CCFLAGS) $(MY_CXXFLAGS)

# link 32 bit exe

$(OUTPUT_DIR)/batterytech_app_x86_32.nexe : $(BT_x86_32_O)
	$(LINK) -o $@ $^ -g -m32 $(LDFLAGS) $(HELLO_WORLD_GLES_LDFLAGS) -L$(NACL_SDK_ROOT)/lib/$(OSNAME)_x86_32_$(BUILD_DIR) $(LINKED_LIBS)

# build 64 bit debug objects

$(BT_x86_64_O): $(BUILD_DIR)/%.x86_64_o : %
	$(CC) -o $@ $< $(BUILD_FLAGS) -m64 $(CCFLAGS) $(MY_CXXFLAGS)

# link 64 bit exe

$(OUTPUT_DIR)/batterytech_app_x86_64.nexe : $(BT_x86_64_O)
	$(LINK) -o $@ $^ -g -m64 $(LDFLAGS) $(HELLO_WORLD_GLES_LDFLAGS) -L$(NACL_SDK_ROOT)/lib/$(OSNAME)_x86_64_$(BUILD_DIR) $(LINKED_LIBS)

# build debug manifest

ALL_TARGETS+=$(OUTPUT_DIR)/batterytech_app.nmf
$(OUTPUT_DIR)/batterytech_app.nmf : $(OUTPUT_DIR)/batterytech_app_x86_32.nexe $(OUTPUT_DIR)/batterytech_app_x86_64.nexe
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


