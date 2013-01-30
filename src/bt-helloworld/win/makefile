# BatteryTech MinGW Win32 builder

SHELL=cmd.exe
CC=mingw32-gcc.exe
CXX=mingw32-g++.exe
STATIC_LINKER=ar.exe
RES_GEN=windres.exe
RELEASE_DIR=Release
DEBUG_DIR=Debug

RELEASE_FLAGS=-O2 -g0
DEBUG_FLAGS=-O0 -g3

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
CPPFLAGS=-c -Wall -I$(SOURCE_DIR) -I$(APP_SOURCE_DIR)
CFLAGS=-c -Wall
LDFLAGS=-mwindows -static-libgcc -static-libstdc++
LINKED_LIBS=Resources\resources.o -L$(BUILD_DIR) -lbatterytech -lgdi32 -lws2_32 -lopengl32 -lwinmm
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
	
APP_SOURCES=\
	AppContext.cpp \
	HelloWorldApp.cpp \
	RenderUtil.cpp \
	WinHooks.cpp
	
	
APP_OBJECTS=$(addprefix $(BUILD_DIR)\, $(addsuffix .o,$(APP_SOURCES)))
APP_DEPS=$(APP_OBJECTS:.o=.d)

APP_EXE=bthelloworld.exe

-include $(APP_DEPS)
-include $(BT_DEPS)

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
	
# Combining all static libs with app+batterytech source
$(OUTPUT_DIR)\$(APP_EXE): $(BT_LIB) $(APP_OBJECTS) Resources\resources.o
	$(CXX) $(LDFLAGS) -o$@ $(APP_OBJECTS) $(LINKED_LIBS)  
	@echo Build Complete: $(OUTPUT_DIR)\$(APP_EXE)
	
$(APP_OBJECTS): $(BUILD_DIR)\\%.o: $(APP_SOURCE_DIR)\\% 
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) -o$@ $<
	$(CXX) $(BUILD_FLAGS) $(CPPFLAGS) -MM -MF$(@:.o=.d) -MT$@ $<

Resources\resources.o: Resources\resources.rc
	$(RES_GEN) Resources\resources.rc Resources\resources.o

clean: cleandebug cleanrelease

cleandebug:
	if exist "$(DEBUG_DIR)" rmdir /q /s $(DEBUG_DIR)
	if exist "Resources\resources.o" del Resources\resources.o
	
cleanrelease:
	if exist "$(RELEASE_DIR)" rmdir /q /s $(RELEASE_DIR)
	if exist "Resources\resources.o" del Resources\resources.o
	
