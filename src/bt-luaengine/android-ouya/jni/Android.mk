# Batterytech Demo App Android makefile


# BatteryTech static lib

include $(CLEAR_VARS)
LOCAL_PATH := ../../batterytech/src

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

LOCAL_CFLAGS := -DANDROID_NDK -DBATTERYTECH_INCLUDE_ASSIMP
LOCAL_MODULE    := batterytech
LOCAL_SRC_FILES := $(my_src_files)	
LOCAL_LDLIBS := -ldl -llog
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/bt-assimp/contrib

include $(BUILD_STATIC_LIBRARY)


# lua

include $(CLEAR_VARS)

LOCAL_PATH := ../../batterytech/src

# List all of your local source files here that you want included in this build
my_src_files :=\
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

LOCAL_CFLAGS := -DANDROID_NDK
LOCAL_MODULE    := lua
LOCAL_SRC_FILES := $(my_src_files)	

include $(BUILD_STATIC_LIBRARY)

# assimp

include $(CLEAR_VARS)

LOCAL_PATH := ../../batterytech/src

# List all of your local source files here that you want included in this build
my_src_files :=\
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

LOCAL_CFLAGS := -DANDROID_NDK -DBATTERYTECH_INCLUDE_ASSIMP
LOCAL_MODULE    := assimp
LOCAL_SRC_FILES := $(my_src_files)	
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/bt-assimp/contrib

include $(BUILD_STATIC_LIBRARY)

# Box2D

include $(CLEAR_VARS)

LOCAL_PATH := ../../batterytech/src

# List all of your local source files here that you want included in this build
my_src_files :=\
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

LOCAL_CFLAGS := -DANDROID_NDK
LOCAL_MODULE    := box2d
LOCAL_SRC_FILES := $(my_src_files)	
LOCAL_C_INCLUDES := $(LOCAL_PATH)

include $(BUILD_STATIC_LIBRARY)

# Now for the app-specific portion

include $(CLEAR_VARS)

LOCAL_PATH := ../app-src

my_src_files :=\
	GameContext.cpp \
	Game.cpp \
	GameLuaBinder.cpp \
	GameUtil.cpp \
	World.cpp \
	ScreenControl.cpp \
	gameobject/GameObject.cpp \
	gameobject/GameObjectLuaBinder.cpp \
	gameobject/ParticleEmitter.cpp \
	level/LevelIO.cpp \
	level/Level.cpp \
	input/TouchInputProcessor.cpp \
	render/WorldRenderer.cpp \
	render/ShadowMap.cpp \
	render/BtDebugRenderer.cpp \
	render/B2DebugRenderer.cpp \
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

LOCAL_MODULE    := batterytech-app
LOCAL_CFLAGS := -DANDROID_NDK -DBATTERYTECH_INCLUDE_ASSIMP -DBATTERYTECH_INCLUDE_BOX2D
LOCAL_C_INCLUDES := ../../batterytech/src ../../batterytech/src/bt-assimp/contrib
LOCAL_SRC_FILES := $(my_src_files)
LOCAL_LDLIBS := -ldl -llog
LOCAL_STATIC_LIBRARIES := batterytech assimp lua box2d
# LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

include $(BUILD_SHARED_LIBRARY)

