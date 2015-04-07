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

LOCAL_CFLAGS := -DANDROID_NDK
LOCAL_MODULE    := batterytech
LOCAL_SRC_FILES := $(my_src_files)	
LOCAL_LDLIBS := -ldl -llog
LOCAL_C_INCLUDES := $(LOCAL_PATH)

include $(BUILD_STATIC_LIBRARY)


# box2d

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

include $(BUILD_STATIC_LIBRARY)

# Now for the app-specific portion

include $(CLEAR_VARS)

LOCAL_PATH := ../app-src

my_src_files :=\
	GameContext.cpp \
	Game.cpp \
	GameUtil.cpp \
	World.cpp \
	gameobject/GameObject.cpp \
	gameobject/GameObjectFactory.cpp \
	gameobject/GameObjectMeta.cpp \
	gameobject/GameObjectProperty.cpp \
	gameobject/GameObjectPropertyMeta.cpp \
	gameobjects/Ball.cpp \
	input/TouchInputProcessor.cpp \
	level/LevelIO.cpp \
	render/WorldRenderer.cpp \
	render/B2DebugRenderer.cpp \
	render/BallRenderer.cpp \
	menus/GameOptionsMenu.cpp \
	menus/SettingsMenu.cpp \
	menus/MenuButtonMenu.cpp \
	menus/SoundcheckMenu.cpp \
	menus/TopMenu.cpp

LOCAL_MODULE    := batterytech-app
LOCAL_CFLAGS := -DANDROID_NDK
LOCAL_C_INCLUDES := ../../batterytech/src
LOCAL_SRC_FILES := $(my_src_files)
LOCAL_LDLIBS := -ldl -llog
LOCAL_STATIC_LIBRARIES := batterytech box2d

include $(BUILD_SHARED_LIBRARY)

