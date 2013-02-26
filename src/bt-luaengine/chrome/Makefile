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
NACL_SDK_ROOT=/nacl_sdk/pepper_23
CHROME_PATH?=Undefined

#
# Defaults
#
NACL_WARNINGS:=-Wno-long-long -Wall -Wswitch-enum -pedantic

RELEASE_DIR=Release
DEBUG_DIR=Debug
X32_LIB_DIR=x86_32
X64_LIB_DIR=x86_64

RELEASE_FLAGS=-O2 -DCHROME -DBATTERYTECH_INCLUDE_ASSIMP -DBATTERYTECH_INCLUDE_BOX2D -I$(SOURCE_DIR) -I$(APP_SOURCE_DIR) -I$(SOURCE_DIR)/bt-assimp/contrib
DEBUG_FLAGS=-g -O0 -DCHROME -DBATTERYTECH_INCLUDE_ASSIMP -DBATTERYTECH_INCLUDE_BOX2D -I$(SOURCE_DIR) -I$(APP_SOURCE_DIR) -I$(SOURCE_DIR)/bt-assimp/contrib
LINKED_LIBS=-lppapi_gles2 -lppapi -lpthread -lbatterytech -lassimp -lbox2d -llua -lz -lnosys 

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
LIB=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-ar -r -c -s
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


APP_SOURCE_DIR=../app-src
SOURCE_DIR=../../batterytech/src

BT_SOURCES=\
	batterytech/batterytech.cpp \
	batterytech/Logger.cpp \
	batterytech/Context.cpp \
	batterytech/VibrationManager.cpp \
	batterytech/decoders/stb_image.c \
	batterytech/decoders/stb_vorbis.c \
	batterytech/audio/PCMSound.cpp \
	batterytech/audio/PCMStream.cpp \
	batterytech/audio/PCMAudioManager.cpp \
	batterytech/audio/AudioManager.cpp \
	batterytech/importers/obj/ObjImporter.cpp \
	batterytech/importers/obj/ObjScene.cpp \
	batterytech/importers/assimp/BTAssimpImporter.cpp \
	batterytech/importers/assimp/BTIOSystem.cpp \
	batterytech/importers/assimp/BTMemoryIOStream.cpp \
	batterytech/network/NetworkManager.cpp \
	batterytech/network/GameConnection.cpp \
	batterytech/network/NetworkMessage.cpp \
	batterytech/render/AssimpAnimator.cpp \
	batterytech/render/RenderNode.cpp \
	batterytech/render/Renderer.cpp \
	batterytech/render/RenderContext.cpp \
	batterytech/render/TextRasterRenderer.cpp \
	batterytech/render/GraphicsConfiguration.cpp \
	batterytech/render/MenuRenderer.cpp \
	batterytech/render/GLModelBinding.cpp \
	batterytech/render/ShaderProgram.cpp \
	batterytech/render/GLResourceManager.cpp \
	batterytech/render/GLObjSceneBinding.cpp \
	batterytech/render/GLAssimpBinding.cpp \
	batterytech/render/GLAssimpMeshBinding.cpp \
	batterytech/render/QuadRenderer.cpp \
	batterytech/render/Texture.cpp \
	batterytech/render/AssetTexture.cpp \
	batterytech/render/AtlasMappedTexture.cpp \
	batterytech/platform/chrome/chromegeneral.cpp \
	batterytech/platform/chrome/chromeboot.cpp \
	batterytech/platform/opengles.cpp \
	batterytech/platform/platformgeneral.cpp \
	batterytech/ui/Button.cpp \
	batterytech/ui/Checkbox.cpp \
	batterytech/ui/Label.cpp \
	batterytech/ui/Layout.cpp \
	batterytech/ui/LinearLayout.cpp \
	batterytech/ui/Menu.cpp \
	batterytech/ui/UIComponent.cpp \
	batterytech/ui/UIManager.cpp \
	batterytech/ui/UIAnimator.cpp \
	batterytech/ui/ScrollableContainer.cpp \
	batterytech/ui/SlideAnimator.cpp \
	batterytech/ui/TextField.cpp \
	batterytech/math/Triangulator.cpp \
	batterytech/util/esTransform.cpp \
	batterytech/util/BitmapUtil.cpp \
	batterytech/util/ByteUtil.cpp \
	batterytech/util/TextFileUtil.cpp \
	batterytech/util/Property.cpp \
	batterytech/util/PropertiesIO.cpp \
	batterytech/util/strx.cpp \
	batterytech/video/VideoManager.cpp
	
BT_x32_0:=$(patsubst %,$(BUILD_DIR)/%.x86_32_o,$(BT_SOURCES))
BT_x64_0:=$(patsubst %,$(BUILD_DIR)/%.x86_64_o,$(BT_SOURCES))
BT_DEPS=$(BT_x32_0:.x86_32_o=.d)

BT_OBJDIRS=$(BUILD_DIR)/batterytech/audio \
	$(BUILD_DIR)/batterytech/decoders \
	$(BUILD_DIR)/batterytech/importers/obj \
	$(BUILD_DIR)/batterytech/importers/assimp \
	$(BUILD_DIR)/batterytech/network \
	$(BUILD_DIR)/batterytech/platform/chrome \
	$(BUILD_DIR)/batterytech/math \
	$(BUILD_DIR)/batterytech/render \
	$(BUILD_DIR)/batterytech/ui \
	$(BUILD_DIR)/batterytech/util \
	$(BUILD_DIR)/batterytech/video
	
BT_LIB_x32=$(BUILD_DIR)/$(X32_LIB_DIR)/libbatterytech.a
BT_LIB_x64=$(BUILD_DIR)/$(X64_LIB_DIR)/libbatterytech.a


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
LUA_LIB_x32=$(BUILD_DIR)/$(X32_LIB_DIR)/liblua.a
LUA_LIB_x64=$(BUILD_DIR)/$(X64_LIB_DIR)/liblua.a


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
ASSIMP_LIB_x32=$(BUILD_DIR)/$(X32_LIB_DIR)/libassimp.a
ASSIMP_LIB_x64=$(BUILD_DIR)/$(X64_LIB_DIR)/libassimp.a

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
	bt-box2d/Dynamics/Contacts/b2ChainAndCircleContact.cpp \
	bt-box2d/Dynamics/Contacts/b2ChainAndPolygonContact.cpp \
	bt-box2d/Dynamics/Contacts/b2CircleContact.cpp \
	bt-box2d/Dynamics/Contacts/b2Contact.cpp \
	bt-box2d/Dynamics/Contacts/b2ContactSolver.cpp \
	bt-box2d/Dynamics/Contacts/b2EdgeAndCircleContact.cpp \
	bt-box2d/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp \
	bt-box2d/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
	bt-box2d/Dynamics/Contacts/b2PolygonContact.cpp \
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
BOX2D_LIB_x32=$(BUILD_DIR)/$(X32_LIB_DIR)/libbox2d.a
BOX2D_LIB_x64=$(BUILD_DIR)/$(X64_LIB_DIR)/libbox2d.a

ZLIB_SOURCES=\
	zlib/adler32.c \
	zlib/compress.c \
	zlib/crc32.c \
	zlib/deflate.c \
	zlib/gzclose.c \
	zlib/gzlib.c \
	zlib/gzread.c \
	zlib/gzwrite.c \
	zlib/infback.c \
	zlib/inffast.c \
	zlib/inflate.c \
	zlib/inftrees.c \
	zlib/trees.c \
	zlib/uncompr.c \
	zlib/zutil.c

ZLIB_x32_0=$(patsubst %,$(BUILD_DIR)/%.x86_32_o,$(ZLIB_SOURCES))
ZLIB_x64_0=$(patsubst %,$(BUILD_DIR)/%.x86_64_o,$(ZLIB_SOURCES))
ZLIB_DEPS=$(ZLIB_x32_0:.x86_32_o=.d)
ZLIB_OBJDIRS=$(BUILD_DIR)/zlib
ZLIB_LIB_x32=$(BUILD_DIR)/$(X32_LIB_DIR)/libz.a
ZLIB_LIB_x64=$(BUILD_DIR)/$(X64_LIB_DIR)/libz.a

# Include header dependency files.
-include $(BT_DEPS)
-include $(LUA_DEPS)
-include $(ASSIMP_DEPS)
-include $(BOX2D_DEPS)
-include $(ZLIB_DEPS)


APP_SOURCES=\
	GameContext.cpp \
	Game.cpp \
	GameLuaBinder.cpp \
	GameUtil.cpp \
	World.cpp \
	ScreenControl.cpp \
	WinHooks.cpp \
	gameobject/GameObject.cpp \
	gameobject/GameObjectLuaBinder.cpp \
	gameobject/ParticleEmitter.cpp \
	level/LevelIO.cpp \
	level/Level.cpp \
	input/TouchInputProcessor.cpp \
	render/WorldRenderer.cpp \
	render/ShadowMap.cpp \
	render/B2DebugRenderer.cpp \
	render/BtDebugRenderer.cpp \
	render/SimpleSpriteRenderer.cpp \
	render/ScreenControlRenderer.cpp \
	render/BatchSpriteRenderer.cpp \
	render/ObjRenderer.cpp \
	render/Camera.cpp \
	render/RenderItem.cpp \
	render/AssimpRenderer.cpp \
	render/GlobalLight.cpp \
	render/LocalLight.cpp \
	render/ParticleEmitterRenderer.cpp \
	script/LuaBinder.cpp \
	menus/ErrorMenu.cpp
	
APP_x32_0=$(patsubst %,$(BUILD_DIR)/%.x86_32_o,$(APP_SOURCES))
APP_x64_0=$(patsubst %,$(BUILD_DIR)/%.x86_64_o,$(APP_SOURCES))
APP_DEPS=$(APP_x64_0:.x86_32_o=.d)
APP_OBJDIRS=$(BUILD_DIR)/gameobject \
	$(BUILD_DIR)/level \
	$(BUILD_DIR)/input \
	$(BUILD_DIR)/render \
	$(BUILD_DIR)/script \
	$(BUILD_DIR)/menus
#
# Default target
#
all: | $(OUTPUT_DIR)
	
$(OUTPUT_DIR):
	$(MKDIR) $(OUTPUT_DIR)
	$(MKDIR) $(BUILD_DIR)/$(X32_LIB_DIR)
	$(MKDIR) $(BUILD_DIR)/$(X64_LIB_DIR)
	

# Box2D

box2d: $(BOX2D_LIB_x32) $(BOX2D_LIB_x64)

$(BOX2D_LIB_x32): $(BOX2D_x32_0)
	$(LIB) $(LIBFLAGS) $@ $(BOX2D_x32_0)
$(BOX2D_LIB_x64): $(BOX2D_x64_0)
	$(LIB) $(LIBFLAGS) $@ $(BOX2D_x64_0)

$(BOX2D_x32_0): $(BUILD_DIR)/%.x86_32_o: $(SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m32 $(CCFLAGS) $(MY_CXXFLAGS)
$(BOX2D_x64_0): $(BUILD_DIR)/%.x86_64_o: $(SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m64 $(CCFLAGS) $(MY_CXXFLAGS)
	
$(BOX2D_x32_0): | $(BOX2D_OBJDIRS)
$(BOX2D_x64_0): | $(BOX2D_OBJDIRS)

$(BOX2D_OBJDIRS):
	$(MKDIR) $(BOX2D_OBJDIRS)

# Lua
lua: $(LUA_LIB_x32) $(LUA_LIB_x64)

$(LUA_LIB_x32): $(LUA_x32_0)
	$(LIB) $(LIBFLAGS) $@ $(LUA_x32_0)
$(LUA_LIB_x64): $(LUA_x64_0)
	$(LIB) $(LIBFLAGS) $@ $(LUA_x64_0)

$(LUA_x32_0): $(BUILD_DIR)/%.x86_32_o: $(SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m32 $(CCFLAGS) $(MY_CXXFLAGS)
$(LUA_x64_0): $(BUILD_DIR)/%.x86_64_o: $(SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m64 $(CCFLAGS) $(MY_CXXFLAGS)

$(LUA_x32_0): | $(LUA_OBJDIRS)
$(LUA_x64_0): | $(LUA_OBJDIRS)

$(LUA_OBJDIRS):
	$(MKDIR) $(LUA_OBJDIRS)


# ZLib
zlib: $(ZLIB_LIB_x32) $(ZLIB_LIB_x64)

$(ZLIB_LIB_x32): $(ZLIB_x32_0)
	$(LIB) $(LIBFLAGS) $@ $(ZLIB_x32_0)
$(ZLIB_LIB_x64): $(ZLIB_x64_0)
	$(LIB) $(LIBFLAGS) $@ $(ZLIB_x64_0)

$(ZLIB_x32_0): $(BUILD_DIR)/%.x86_32_o: $(SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m32 $(CCFLAGS) $(MY_CXXFLAGS)
$(ZLIB_x64_0): $(BUILD_DIR)/%.x86_64_o: $(SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m64 $(CCFLAGS) $(MY_CXXFLAGS)

$(ZLIB_x32_0): | $(ZLIB_OBJDIRS)
$(ZLIB_x64_0): | $(ZLIB_OBJDIRS)

$(ZLIB_OBJDIRS):
	$(MKDIR) $(ZLIB_OBJDIRS)
	
	
# Open Asset Importer Library
assimp: $(ASSIMP_LIB_x32) $(ASSIMP_LIB_x64)

$(ASSIMP_LIB_x32): $(ASSIMP_x32_0)
	$(LIB) $(LIBFLAGS) $@ $(ASSIMP_x32_0)
$(ASSIMP_LIB_x64): $(ASSIMP_x64_0)
	$(LIB) $(LIBFLAGS) $@ $(ASSIMP_x64_0)

$(ASSIMP_x32_0): $(BUILD_DIR)/%.x86_32_o: $(SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m32 $(CCFLAGS) $(MY_CXXFLAGS)
$(ASSIMP_x64_0): $(BUILD_DIR)/%.x86_64_o: $(SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m64 $(CCFLAGS) $(MY_CXXFLAGS)
	
$(ASSIMP_x32_0): | $(ASSIMP_OBJDIRS)
$(ASSIMP_x64_0): | $(ASSIMP_OBJDIRS)

$(ASSIMP_OBJDIRS):
	$(MKDIR) $(ASSIMP_OBJDIRS)

# BatteryTech SDK
bt: $(BT_LIB_x32) $(BT_LIB_x64)

$(BT_LIB_x32): $(BT_x32_0)
	$(LIB) $(LIBFLAGS) $@ $(BT_x32_0)
$(BT_LIB_x64): $(BT_x64_0)
	$(LIB) $(LIBFLAGS) $@ $(BT_x64_0)

$(BT_x32_0): $(BUILD_DIR)/%.x86_32_o: $(SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m32 $(CCFLAGS) $(MY_CXXFLAGS)
$(BT_x64_0): $(BUILD_DIR)/%.x86_64_o: $(SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m64 $(CCFLAGS) $(MY_CXXFLAGS)
	
$(BT_x32_0): | $(BT_OBJDIRS)
$(BT_x64_0): | $(BT_OBJDIRS)

$(BT_OBJDIRS):
	$(MKDIR) $(BT_OBJDIRS)
	
	
# Engine

# build 32 bit objects

$(APP_x32_0): | $(APP_OBJDIRS)
$(APP_x64_0): | $(APP_OBJDIRS)

$(APP_OBJDIRS):
	$(MKDIR) $(APP_OBJDIRS)

$(APP_x32_0): $(BUILD_DIR)/%.x86_32_o : $(APP_SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m32 $(CCFLAGS) $(MY_CXXFLAGS)

# link 32 bit exe

$(OUTPUT_DIR)/batterytech_app_x86_32.nexe : $(ASSIMP_LIB_x32) $(LUA_LIB_x32) $(BOX2D_LIB_x32) $(ZLIB_LIB_x32) $(BT_LIB_x32) $(APP_x32_0) 
	$(LINK) -o $@ $^ -g -m32 $(LDFLAGS) $(HELLO_WORLD_GLES_LDFLAGS) -L$(NACL_SDK_ROOT)/lib/$(OSNAME)_x86_32_$(BUILD_DIR) -L$(BUILD_DIR)/$(X32_LIB_DIR) $(LINKED_LIBS)

# build 64 bit debug objects

$(APP_x64_0): $(BUILD_DIR)/%.x86_64_o : $(APP_SOURCE_DIR)/%
	$(CC) -o $@ $< $(BUILD_FLAGS) -m64 $(CCFLAGS) $(MY_CXXFLAGS)

# link 64 bit exe

$(OUTPUT_DIR)/batterytech_app_x86_64.nexe : $(ASSIMP_LIB_x64) $(LUA_LIB_x64) $(BOX2D_LIB_x64) $(ZLIB_LIB_x64) $(BT_LIB_x64) $(APP_x64_0)
	$(LINK) -o $@ $^ -g -m64 $(LDFLAGS) $(HELLO_WORLD_GLES_LDFLAGS) -L$(NACL_SDK_ROOT)/lib/$(OSNAME)_x86_64_$(BUILD_DIR) -L$(BUILD_DIR)/$(X64_LIB_DIR) $(LINKED_LIBS)

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


