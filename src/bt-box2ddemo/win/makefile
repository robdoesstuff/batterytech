# Bleeps MinGW Win32 builder

SHELL=cmd.exe
MINGW_DIR=c:\mingw
CC=$(MINGW_DIR)\bin\gcc.exe
CXX=$(MINGW_DIR)\bin\g++.exe
STATIC_LINKER=$(MINGW_DIR)\bin\ar.exe
RES_GEN=$(MINGW_DIR)\bin\windres.exe
RELEASE_DIR=Release
DEBUG_DIR=Debug
#OUTPUT_DIR=$(RELEASE_DIR)

RELEASE_FLAGS=-O2 -g0
DEBUG_FLAGS=-O0 -g3
#BUILD_FLAGS=$(RELEASE_FLAGS)
#BUILD_DIR=$(RELEASE_DIR)

ifeq (${DEBUG},true)
	BUILD_FLAGS=$(DEBUG_FLAGS)
	OUTPUT_DIR=$(DEBUG_DIR)
	BUILD_DIR=$(DEBUG_DIR)
else
	BUILD_FLAGS=$(RELEASE_FLAGS)
	OUTPUT_DIR=$(RELEASE_DIR)
	BUILD_DIR=$(RELEASE_DIR)
endif


SOURCE_DIR=..\slyonball\src
CPPFLAGS=-c -Wall -I$(SOURCE_DIR)\Bullet -I$(SOURCE_DIR)\assimp\contrib -I$(SOURCE_DIR)
CFLAGS=-c -Wall
LDFLAGS=-mwindows -static-libgcc -static-libstdc++
LINKED_LIBS=Resources\resources.o -L$(BUILD_DIR) -llua -lassimp -lgdi32 -lws2_32 -lopengl32 -lwinmm
ARFLAGS=-r -c -s

LUA_SOURCES=\
	lua\lapi.c \
	lua\lauxlib.c \
	lua\lbaselib.c \
	lua\lcode.c \
	lua\ldblib.c \
	lua\ldebug.c \
	lua\ldo.c \
	lua\ldump.c \
	lua\lfunc.c \
	lua\lgc.c \
	lua\linit.c \
	lua\liolib.c \
	lua\llex.c \
	lua\lmathlib.c \
	lua\lmem.c \
	lua\loadlib.c \
	lua\lobject.c \
	lua\lopcodes.c \
	lua\loslib.c \
	lua\lparser.c \
	lua\lstate.c \
	lua\lstring.c \
	lua\lstrlib.c \
	lua\ltable.c \
	lua\ltablib.c \
	lua\ltm.c \
	lua\lua.c \
	lua\luac.c \
	lua\lundump.c \
	lua\lvm.c \
	lua\lzio.c \
	lua\print.c

LUA_OBJECTS=$(addprefix $(BUILD_DIR)\, $(addsuffix .o,$(LUA_SOURCES)))
LUA_DEPS=$(LUA_OBJECTS:.o=.d)
LUA_OBJDIRS=$(BUILD_DIR)\lua

LUA_LIB=$(BUILD_DIR)\liblua.a


ASSIMP_SOURCES=\
	assimp\code\Assimp.cpp \
	assimp\code\AssimpPCH.cpp \
	assimp\code\BAIFileImporter.cpp \
	assimp\code\BaseImporter.cpp \
	assimp\code\BaseProcess.cpp \
	assimp\code\ColladaLoader.cpp \
	assimp\code\ColladaParser.cpp \
	assimp\code\DefaultIOStream.cpp \
	assimp\code\DefaultIOSystem.cpp \
	assimp\code\DefaultLogger.cpp \
	assimp\code\Importer.cpp \
	assimp\code\MaterialSystem.cpp \
	assimp\code\ObjFileImporter.cpp \
	assimp\code\ObjFileMtlImporter.cpp \
	assimp\code\ObjFileParser.cpp \
	assimp\code\ScenePreprocessor.cpp \
	assimp\code\SkeletonMeshBuilder.cpp \
	assimp\code\SpatialSort.cpp \
	assimp\code\LimitBoneWeightsProcess.cpp \
	assimp\contrib\irrXML\irrXML.cpp \
	assimp\contrib\ConvertUTF\ConvertUTF.c

ASSIMP_OBJECTS=$(addprefix $(BUILD_DIR)\, $(addsuffix .o,$(ASSIMP_SOURCES)))
ASSIMP_DEPS=$(ASSIMP_OBJECTS:.o=.d)
ASSIMP_OBJDIRS=$(BUILD_DIR)\assimp\code \
	$(BUILD_DIR)\assimp\contrib\irrXML \
	$(BUILD_DIR)\assimp\contrib\ConvertUTF

ASSIMP_LIB=$(BUILD_DIR)\libassimp.a

GAME_SOURCES=\
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
	batterytech\render\Texture.cpp \
	batterytech\platform\win32\win32general.cpp \
	batterytech\platform\win32\boot.cpp \
	batterytech\platform\win32\WinSound.cpp \
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
	batterytech\util\strx.cpp
	
GAME_SOURCES +=\
	game\Game.cpp \
	game\GameLuaBinder.cpp \
	game\GameUtil.cpp \
	game\World.cpp \
	game\ScreenControl.cpp \
	game\gameobject\GameObject.cpp \
	game\gameobject\GameObjectLuaBinder.cpp \
	game\level\LevelIO.cpp \
	game\level\Level.cpp \
	game\input\TouchInputProcessor.cpp \
	game\render\WorldRenderer.cpp \
	game\render\ShadowMap.cpp \
	game\render\BtDebugRenderer.cpp \
	game\render\SimpleSpriteRenderer.cpp \
	game\render\ScreenControlRenderer.cpp \
	game\render\BatchSpriteRenderer.cpp \
	game\render\ObjRenderer.cpp \
	game\render\LevelRenderer.cpp \
	game\render\Camera.cpp \
	game\render\IndicatorRenderer.cpp \
	game\render\RenderItem.cpp \
	game\render\AssimpRenderer.cpp \
	game\render\GlobalLight.cpp \
	game\render\LocalLight.cpp \
	game\script\LuaBinder.cpp \
	game\menus\GameOptionsMenu.cpp \
	game\menus\SettingsMenu.cpp \
	game\menus\MenuButtonMenu.cpp \
	game\menus\ErrorMenu.cpp \
	game\menus\TopMenu.cpp
	
GAME_OBJECTS=$(addprefix $(BUILD_DIR)\, $(addsuffix .o,$(GAME_SOURCES)))
GAME_DEPS=$(GAME_OBJECTS:.o=.d)
GAME_OBJDIRS=$(BUILD_DIR)\batterytech\audio \
	$(BUILD_DIR)\batterytech\decoders \
	$(BUILD_DIR)\batterytech\importers\obj \
	$(BUILD_DIR)\batterytech\importers\assimp \
	$(BUILD_DIR)\batterytech\network \
	$(BUILD_DIR)\batterytech\platform\win32 \
	$(BUILD_DIR)\batterytech\math \
	$(BUILD_DIR)\batterytech\render \
	$(BUILD_DIR)\batterytech\ui \
	$(BUILD_DIR)\batterytech\util \
	$(BUILD_DIR)\game\gameobject \
	$(BUILD_DIR)\game\level \
	$(BUILD_DIR)\game\input \
	$(BUILD_DIR)\game\render \
	$(BUILD_DIR)\game\script \
	$(BUILD_DIR)\game\menus

GAME_EXE=slyonball.exe

-include $(LUA_DEPS)
-include $(ASSIMP_DEPS)
-include $(GAME_DEPS)

all: $(OUTPUT_DIR)\$(GAME_EXE)

lua: $(LUA_LIB)

$(LUA_LIB): $(LUA_OBJECTS)
	$(STATIC_LINKER) $(ARFLAGS) $@ $(LUA_OBJECTS)

$(LUA_OBJECTS): $(BUILD_DIR)\\%.o: $(SOURCE_DIR)\\%
	$(CC) $(BUILD_FLAGS) $(CFLAGS) $< -o $@
	$(CC) $(BUILD_FLAGS) $(CFLAGS) -MM -MF$(@:.o=.d) -MT$@ $<

$(LUA_OBJECTS): | $(LUA_OBJDIRS)

$(LUA_OBJDIRS):
	-md $(LUA_OBJDIRS)
	
assimp: $(ASSIMP_LIB)

$(ASSIMP_LIB): $(ASSIMP_OBJECTS)
	$(STATIC_LINKER) $(ARFLAGS) $@ $(ASSIMP_OBJECTS)

$(ASSIMP_OBJECTS): $(BUILD_DIR)\\%.o: $(SOURCE_DIR)\\%
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) $< -o $@
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) -MM -MF$(@:.o=.d) -MT$@ $<
	
$(ASSIMP_OBJECTS): | $(ASSIMP_OBJDIRS)

$(ASSIMP_OBJDIRS):
	-md $(ASSIMP_OBJDIRS)

# Combining all static libs with game+batterytech source
$(OUTPUT_DIR)\$(GAME_EXE): $(LUA_LIB) $(ASSIMP_LIB) $(GAME_OBJECTS) Resources\resources.o
	$(CXX) $(LDFLAGS) -o$@ $(GAME_OBJECTS) $(LINKED_LIBS)  
	@echo Build Complete: $(OUTPUT_DIR)\$(GAME_EXE)
	
$(GAME_OBJECTS): $(BUILD_DIR)\\%.o: $(SOURCE_DIR)\\% 
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) -o$@ $<
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) -MM -MF$(@:.o=.d) -MT$@ $<
	
$(GAME_OBJECTS): | $(GAME_OBJDIRS)

$(GAME_OBJDIRS):
	-md $(GAME_OBJDIRS)

Resources\resources.o: Resources\resources.rc
	$(RES_GEN) Resources\resources.rc Resources\resources.o

clean: cleandebug cleanrelease

cleandebug:
	if exist "$(DEBUG_DIR)" rmdir /q /s $(DEBUG_DIR)
	if exist "Resources\resources.o" del Resources\resources.o
	
cleanrelease:
	if exist "$(RELEASE_DIR)" rmdir /q /s $(RELEASE_DIR)
	if exist "Resources\resources.o" del Resources\resources.o
	
