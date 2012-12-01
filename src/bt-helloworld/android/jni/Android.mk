# Batterytech Demo App Android makefile

include $(CLEAR_VARS)

LOCAL_PATH := ../../batterytech/src

# List all of your local source files here that you want included in this build
my_src_files :=\
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
	batterytech/platform/android/androidgeneral.cpp \
	batterytech/platform/android/androidboot.cpp \
	batterytech/platform/android/importgl.cpp \
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

LOCAL_CFLAGS := -DANDROID_NDK
LOCAL_MODULE    := batterytech
LOCAL_SRC_FILES := $(my_src_files)	
LOCAL_LDLIBS := -ldl -llog

include $(BUILD_STATIC_LIBRARY)

# Now for the app-specific portion

include $(CLEAR_VARS)

LOCAL_PATH := ../app-src

my_src_files :=\
	AppContext.cpp \
	HelloWorldApp.cpp \
	RenderUtil.cpp

LOCAL_MODULE    := batterytech-app
LOCAL_CFLAGS := -DANDROID_NDK \
                -I ../../batterytech/src
LOCAL_SRC_FILES := $(my_src_files)
LOCAL_LDLIBS := -ldl -llog
LOCAL_STATIC_LIBRARIES := batterytech
# LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

include $(BUILD_SHARED_LIBRARY)

