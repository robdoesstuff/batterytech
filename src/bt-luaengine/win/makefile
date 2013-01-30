# BatteryTech MinGW Win32 builder

SHELL=cmd.exe
CC=mingw32-gcc.exe
CXX=mingw32-g++.exe
STATIC_LINKER=ar.exe
RES_GEN=windres.exe
RELEASE_DIR=Release
DEBUG_DIR=Debug

RELEASE_FLAGS=-O2 -g0 -DBATTERYTECH_INCLUDE_ASSIMP -DBATTERYTECH_INCLUDE_BOX2D
DEBUG_FLAGS=-O0 -g3 -DBATTERYTECH_INCLUDE_ASSIMP -DBATTERYTECH_INCLUDE_BOX2D

ifeq (${DEBUG},true)
	BUILD_FLAGS=$(DEBUG_FLAGS)
	OUTPUT_DIR=$(DEBUG_DIR)
	BUILD_DIR=$(DEBUG_DIR)
else
	BUILD_FLAGS=$(RELEASE_FLAGS)
	OUTPUT_DIR=$(RELEASE_DIR)
	BUILD_DIR=$(RELEASE_DIR)
endif


APP_SOURCE_DIR=..\app-src
SOURCE_DIR=..\..\batterytech\src
CPPFLAGS=-c -Wall -I$(SOURCE_DIR) -I$(APP_SOURCE_DIR) -I$(SOURCE_DIR)\bt-assimp\contrib
CFLAGS=-c -Wall 
LDFLAGS=-mwindows -static-libgcc -static-libstdc++
# important - order matters to GCC when one static lib is dep on another, so always do -lbatterytech first!
LINKED_LIBS=Resources\resources.o -L$(BUILD_DIR) -lbatterytech -lassimp -lbox2d -llua -lgdi32 -lws2_32 -lopengl32 -lwinmm
ARFLAGS=-r -c -s


BT_SOURCES=\
	batterytech\batterytech.cpp \
	batterytech\Logger.cpp \
	batterytech\Context.cpp \
	batterytech\VibrationManager.cpp \
	batterytech\decoders\stb_image.c \
	batterytech\decoders\stb_vorbis.c \
	batterytech\audio\PCMSound.cpp \
	batterytech\audio\PCMStream.cpp \
	batterytech\audio\PCMAudioManager.cpp \
	batterytech\audio\AudioManager.cpp \
	batterytech\importers\obj\ObjImporter.cpp \
	batterytech\importers\obj\ObjScene.cpp \
	batterytech\importers\assimp\BTAssimpImporter.cpp \
	batterytech\importers\assimp\BTIOSystem.cpp \
	batterytech\importers\assimp\BTMemoryIOStream.cpp \
	batterytech\network\NetworkManager.cpp \
	batterytech\network\GameConnection.cpp \
	batterytech\network\NetworkMessage.cpp \
	batterytech\render\AssimpAnimator.cpp \
	batterytech\render\RenderNode.cpp \
	batterytech\render\Renderer.cpp \
	batterytech\render\RenderContext.cpp \
	batterytech\render\TextRasterRenderer.cpp \
	batterytech\render\GraphicsConfiguration.cpp \
	batterytech\render\MenuRenderer.cpp \
	batterytech\render\GLModelBinding.cpp \
	batterytech\render\ShaderProgram.cpp \
	batterytech\render\GLResourceManager.cpp \
	batterytech\render\GLObjSceneBinding.cpp \
	batterytech\render\GLAssimpBinding.cpp \
	batterytech\render\GLAssimpMeshBinding.cpp \
	batterytech\render\QuadRenderer.cpp \
	batterytech\render\Texture.cpp \
	batterytech\render\AssetTexture.cpp \
	batterytech\render\AtlasMappedTexture.cpp \
	batterytech\platform\win32\win32general.cpp \
	batterytech\platform\win32\winboot.cpp \
	batterytech\platform\win32\WinSound.cpp \
	batterytech\platform\glew.c \
	batterytech\platform\opengles.cpp \
	batterytech\platform\platformgeneral.cpp \
	batterytech\ui\Button.cpp \
	batterytech\ui\Checkbox.cpp \
	batterytech\ui\Label.cpp \
	batterytech\ui\Layout.cpp \
	batterytech\ui\LinearLayout.cpp \
	batterytech\ui\Menu.cpp \
	batterytech\ui\UIComponent.cpp \
	batterytech\ui\UIManager.cpp \
	batterytech\ui\UIAnimator.cpp \
	batterytech\ui\ScrollableContainer.cpp \
	batterytech\ui\SlideAnimator.cpp \
	batterytech\ui\TextField.cpp \
	batterytech\math\Triangulator.cpp \
	batterytech\util\esTransform.cpp \
	batterytech\util\BitmapUtil.cpp \
	batterytech\util\ByteUtil.cpp \
	batterytech\util\TextFileUtil.cpp \
	batterytech\util\Property.cpp \
	batterytech\util\PropertiesIO.cpp \
	batterytech\util\strx.cpp \
	batterytech\video\VideoManager.cpp
	
BT_OBJECTS=$(addprefix $(BUILD_DIR)\, $(addsuffix .o,$(BT_SOURCES)))
BT_DEPS=$(BT_OBJECTS:.o=.d)
BT_OBJDIRS=$(BUILD_DIR)\batterytech\audio \
	$(BUILD_DIR)\batterytech\decoders \
	$(BUILD_DIR)\batterytech\importers\obj \
	$(BUILD_DIR)\batterytech\importers\assimp \
	$(BUILD_DIR)\batterytech\network \
	$(BUILD_DIR)\batterytech\platform\win32 \
	$(BUILD_DIR)\batterytech\math \
	$(BUILD_DIR)\batterytech\render \
	$(BUILD_DIR)\batterytech\ui \
	$(BUILD_DIR)\batterytech\util \
	$(BUILD_DIR)\batterytech\video
	
BT_LIB=$(BUILD_DIR)\libbatterytech.a


LUA_SOURCES=\
	bt-lua\lapi.c \
	bt-lua\lauxlib.c \
	bt-lua\lbaselib.c \
	bt-lua\lcode.c \
	bt-lua\ldblib.c \
	bt-lua\ldebug.c \
	bt-lua\ldo.c \
	bt-lua\ldump.c \
	bt-lua\lfunc.c \
	bt-lua\lgc.c \
	bt-lua\linit.c \
	bt-lua\liolib.c \
	bt-lua\llex.c \
	bt-lua\lmathlib.c \
	bt-lua\lmem.c \
	bt-lua\loadlib.c \
	bt-lua\lobject.c \
	bt-lua\lopcodes.c \
	bt-lua\loslib.c \
	bt-lua\lparser.c \
	bt-lua\lstate.c \
	bt-lua\lstring.c \
	bt-lua\lstrlib.c \
	bt-lua\ltable.c \
	bt-lua\ltablib.c \
	bt-lua\ltm.c \
	bt-lua\lua.c \
	bt-lua\luac.c \
	bt-lua\lundump.c \
	bt-lua\lvm.c \
	bt-lua\lzio.c \
	bt-lua\print.c

LUA_OBJECTS=$(addprefix $(BUILD_DIR)\, $(addsuffix .o,$(LUA_SOURCES)))
LUA_DEPS=$(LUA_OBJECTS:.o=.d)
LUA_OBJDIRS=$(BUILD_DIR)\bt-lua

LUA_LIB=$(BUILD_DIR)\liblua.a

ASSIMP_SOURCES=\
	bt-assimp\code\Assimp.cpp \
	bt-assimp\code\AssimpPCH.cpp \
	bt-assimp\code\BAIFileImporter.cpp \
	bt-assimp\code\BaseImporter.cpp \
	bt-assimp\code\BaseProcess.cpp \
	bt-assimp\code\ColladaLoader.cpp \
	bt-assimp\code\ColladaParser.cpp \
	bt-assimp\code\DefaultIOStream.cpp \
	bt-assimp\code\DefaultIOSystem.cpp \
	bt-assimp\code\DefaultLogger.cpp \
	bt-assimp\code\Importer.cpp \
	bt-assimp\code\MaterialSystem.cpp \
	bt-assimp\code\ObjFileImporter.cpp \
	bt-assimp\code\ObjFileMtlImporter.cpp \
	bt-assimp\code\ObjFileParser.cpp \
	bt-assimp\code\ScenePreprocessor.cpp \
	bt-assimp\code\SkeletonMeshBuilder.cpp \
	bt-assimp\code\SpatialSort.cpp \
	bt-assimp\code\LimitBoneWeightsProcess.cpp \
	bt-assimp\contrib\irrXML\irrXML.cpp \
	bt-assimp\contrib\ConvertUTF\ConvertUTF.c

ASSIMP_OBJECTS=$(addprefix $(BUILD_DIR)\, $(addsuffix .o,$(ASSIMP_SOURCES)))
ASSIMP_DEPS=$(ASSIMP_OBJECTS:.o=.d)
ASSIMP_OBJDIRS=$(BUILD_DIR)\bt-assimp\code \
	$(BUILD_DIR)\bt-assimp\contrib\irrXML \
	$(BUILD_DIR)\bt-assimp\contrib\ConvertUTF

ASSIMP_LIB=$(BUILD_DIR)\libassimp.a

BOX2D_SOURCES=\
	bt-box2d\Collision\b2BroadPhase.cpp \
	bt-box2d\Collision\b2CollideCircle.cpp \
	bt-box2d\Collision\b2CollideEdge.cpp \
	bt-box2d\Collision\b2CollidePolygon.cpp \
	bt-box2d\Collision\b2Collision.cpp \
	bt-box2d\Collision\b2Distance.cpp \
	bt-box2d\Collision\b2DynamicTree.cpp \
	bt-box2d\Collision\b2TimeOfImpact.cpp \
	bt-box2d\Collision\Shapes\b2ChainShape.cpp \
	bt-box2d\Collision\Shapes\b2CircleShape.cpp \
	bt-box2d\Collision\Shapes\b2EdgeShape.cpp \
	bt-box2d\Collision\Shapes\b2PolygonShape.cpp \
	bt-box2d\Common\b2BlockAllocator.cpp \
	bt-box2d\Common\b2Draw.cpp \
	bt-box2d\Common\b2Math.cpp \
	bt-box2d\Common\b2Settings.cpp \
	bt-box2d\Common\b2StackAllocator.cpp \
	bt-box2d\Common\b2Timer.cpp \
	bt-box2d\Dynamics\b2Body.cpp \
	bt-box2d\Dynamics\b2ContactManager.cpp \
	bt-box2d\Dynamics\b2Fixture.cpp \
	bt-box2d\Dynamics\b2Island.cpp \
	bt-box2d\Dynamics\b2World.cpp \
	bt-box2d\Dynamics\b2WorldCallbacks.cpp \
	bt-box2d\Dynamics\Contacts\b2ChainAndCircleContact.cpp \
	bt-box2d\Dynamics\Contacts\b2ChainAndPolygonContact.cpp \
	bt-box2d\Dynamics\Contacts\b2CircleContact.cpp \
	bt-box2d\Dynamics\Contacts\b2Contact.cpp \
	bt-box2d\Dynamics\Contacts\b2ContactSolver.cpp \
	bt-box2d\Dynamics\Contacts\b2EdgeAndCircleContact.cpp \
	bt-box2d\Dynamics\Contacts\b2EdgeAndPolygonContact.cpp \
	bt-box2d\Dynamics\Contacts\b2PolygonAndCircleContact.cpp \
	bt-box2d\Dynamics\Contacts\b2PolygonContact.cpp \
	bt-box2d\Dynamics\Joints\b2DistanceJoint.cpp \
	bt-box2d\Dynamics\Joints\b2FrictionJoint.cpp \
	bt-box2d\Dynamics\Joints\b2GearJoint.cpp \
	bt-box2d\Dynamics\Joints\b2Joint.cpp \
	bt-box2d\Dynamics\Joints\b2MouseJoint.cpp \
	bt-box2d\Dynamics\Joints\b2PrismaticJoint.cpp \
	bt-box2d\Dynamics\Joints\b2PulleyJoint.cpp \
	bt-box2d\Dynamics\Joints\b2RevoluteJoint.cpp \
	bt-box2d\Dynamics\Joints\b2RopeJoint.cpp \
	bt-box2d\Dynamics\Joints\b2WeldJoint.cpp \
	bt-box2d\Dynamics\Joints\b2WheelJoint.cpp
	
BOX2D_OBJECTS=$(addprefix $(BUILD_DIR)\, $(addsuffix .o,$(BOX2D_SOURCES)))
BOX2D_DEPS=$(BOX2D_OBJECTS:.o=.d)
BOX2D_OBJDIRS=$(BUILD_DIR)\bt-box2d\Collision \
	$(BUILD_DIR)\bt-box2d\Collision\Shapes \
	$(BUILD_DIR)\bt-box2d\Common \
	$(BUILD_DIR)\bt-box2d\Dynamics \
	$(BUILD_DIR)\bt-box2d\Dynamics\Contacts \
	$(BUILD_DIR)\bt-box2d\Dynamics\Joints \
	$(BUILD_DIR)\bt-box2d\Rope

BOX2D_LIB=$(BUILD_DIR)\libbox2d.a
	
APP_SOURCES=\
	GameContext.cpp \
	Game.cpp \
	GameLuaBinder.cpp \
	GameUtil.cpp \
	World.cpp \
	ScreenControl.cpp \
	WinHooks.cpp \
	gameobject\GameObject.cpp \
	gameobject\GameObjectLuaBinder.cpp \
	gameobject\ParticleEmitter.cpp \
	level\LevelIO.cpp \
	level\Level.cpp \
	input\TouchInputProcessor.cpp \
	render\WorldRenderer.cpp \
	render\ShadowMap.cpp \
	render\B2DebugRenderer.cpp \
	render\BtDebugRenderer.cpp \
	render\SimpleSpriteRenderer.cpp \
	render\ScreenControlRenderer.cpp \
	render\BatchSpriteRenderer.cpp \
	render\ObjRenderer.cpp \
	render\Camera.cpp \
	render\RenderItem.cpp \
	render\AssimpRenderer.cpp \
	render\GlobalLight.cpp \
	render\LocalLight.cpp \
	render\ParticleEmitterRenderer.cpp \
	script\LuaBinder.cpp \
	menus\ErrorMenu.cpp
	
APP_OBJECTS=$(addprefix $(BUILD_DIR)\, $(addsuffix .o,$(APP_SOURCES)))
APP_DEPS=$(APP_OBJECTS:.o=.d)
APP_OBJDIRS=$(BUILD_DIR)\gameobject \
	$(BUILD_DIR)\level \
	$(BUILD_DIR)\input \
	$(BUILD_DIR)\render \
	$(BUILD_DIR)\script \
	$(BUILD_DIR)\menus
	
APP_EXE=btengine.exe

-include $(APP_DEPS)
-include $(BT_DEPS)
-include $(LUA_DEPS)
-include $(ASSIMP_DEPS)

all: $(OUTPUT_DIR)\$(APP_EXE)
	
bt: $(BT_LIB)

# BatteryTech lib
$(BT_LIB): $(BT_OBJECTS)
	$(STATIC_LINKER) $(ARFLAGS) $@ $(BT_OBJECTS)

$(BT_OBJECTS): $(BUILD_DIR)\\%.o: $(SOURCE_DIR)\\% 
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) -o$@ $<
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) -MM -MF$(@:.o=.d) -MT$@ $< 
	
$(BT_OBJECTS): | $(BT_OBJDIRS)

$(BT_OBJDIRS):
	-md $(BT_OBJDIRS)

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

# Combining all static libs with app+batterytech source
$(OUTPUT_DIR)\$(APP_EXE): $(ASSIMP_LIB) $(LUA_LIB) $(BOX2D_LIB) $(BT_LIB) $(APP_OBJECTS) Resources\resources.o
	$(CXX) $(LDFLAGS) -o$@ $(APP_OBJECTS) $(LINKED_LIBS)  
	@echo Build Complete: $(OUTPUT_DIR)\$(APP_EXE)
	
$(APP_OBJECTS): $(BUILD_DIR)\\%.o: $(APP_SOURCE_DIR)\\% 
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) -o$@ $<
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) -MM -MF$(@:.o=.d) -MT$@ $<

$(APP_OBJECTS): | $(APP_OBJDIRS)

$(APP_OBJDIRS):
	-md $(APP_OBJDIRS)
	
Resources\resources.o: Resources\resources.rc
	$(RES_GEN) Resources\resources.rc Resources\resources.o

clean: cleandebug cleanrelease

cleandebug:
	if exist "$(DEBUG_DIR)" rmdir /q /s $(DEBUG_DIR)
	if exist "Resources\resources.o" del Resources\resources.o
	
cleanrelease:
	if exist "$(RELEASE_DIR)" rmdir /q /s $(RELEASE_DIR)
	if exist "Resources\resources.o" del Resources\resources.o
	
