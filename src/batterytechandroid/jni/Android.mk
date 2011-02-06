# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)/src

include $(CLEAR_VARS)

LOCAL_CFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
                -I src

LOCAL_MODULE    := demo-app
LOCAL_SRC_FILES :=\
	batterytech/batterytech.cpp \
	batterytech/Logger.cpp \
	batterytech/Context.cpp \
	batterytech/VibrationManager.cpp \
	batterytech/decoders/stb_image.c \
	batterytech/decoders/stb_vorbis.c \
	batterytech/sound/PCMSound.cpp \
	batterytech/sound/PCMStream.cpp \
	batterytech/sound/PCMSoundManager.cpp \
	batterytech/sound/SoundManager.cpp \
	batterytech/network/NetworkManager.cpp \
	batterytech/network/GameConnection.cpp \
	batterytech/network/NetworkMessage.cpp \
	batterytech/render/Renderer.cpp \
	batterytech/render/TextRasterRenderer.cpp \
	batterytech/render/GraphicsConfiguration.cpp \
	batterytech/render/MenuRenderer.cpp \
	batterytech/platform/android/androidgeneral.cpp \
	batterytech/platform/android/boot.cpp \
	batterytech/platform/android/importgl.c \
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
	batterytech/ui/SlideAnimator.cpp \
	batterytech/ui/TextField.cpp \
	batterytech/menus/MainMenu.cpp \
	batterytech/menus/OptionsMenu.cpp \
	batterytech/util/Triangulator.cpp \
	batterytech/util/ByteUtil.cpp \
	batterytech/util/TextFileUtil.cpp \
	batterytech/util/Property.cpp \
	batterytech/util/PropertiesIO.cpp \
	batterytech/util/strx.cpp
	
LOCAL_SRC_FILES +=\
	demo-app/Game.cpp \
	demo-app/GameUtil.cpp \
	demo-app/World.cpp \
	demo-app/gameobject/GameObject.cpp \
	demo-app/gameobject/GameObjectFactory.cpp \
	demo-app/gameobject/GameObjectMeta.cpp \
	demo-app/gameobject/GameObjectProperty.cpp \
	demo-app/gameobject/GameObjectPropertyMeta.cpp \
	demo-app/level/LevelIO.cpp \
	demo-app/input/TouchInputProcessor.cpp \
	demo-app/render/WorldRenderer.cpp \
	demo-app/render/B2DebugRenderer.cpp \
	demo-app/render/SimpleSpriteRenderer.cpp \
	demo-app/render/PinballRenderer.cpp \
	demo-app/gameobjects/Pinball.cpp \
	demo-app/menus/GameOptionsMenu.cpp \
	demo-app/menus/SettingsMenu.cpp \
	demo-app/menus/MenuButtonMenu.cpp
	
	
LOCAL_SRC_FILES +=\
	Box2D/Collision/b2BroadPhase.cpp \
	Box2D/Collision/b2CollideCircle.cpp \
	Box2D/Collision/b2CollidePolygon.cpp \
	Box2D/Collision/b2Collision.cpp \
	Box2D/Collision/b2Distance.cpp \
	Box2D/Collision/b2DynamicTree.cpp \
	Box2D/Collision/b2TimeofImpact.cpp \
	Box2D/Collision/Shapes/b2CircleShape.cpp \
	Box2D/Collision/Shapes/b2PolygonShape.cpp \
	Box2D/Common/b2BlockAllocator.cpp \
	Box2D/Common/b2Math.cpp \
	Box2D/Common/b2Settings.cpp \
	Box2D/Common/b2StackAllocator.cpp \
	Box2D/Dynamics/b2Body.cpp \
	Box2D/Dynamics/b2ContactManager.cpp \
	Box2D/Dynamics/b2Fixture.cpp \
	Box2D/Dynamics/b2Island.cpp \
	Box2D/Dynamics/b2World.cpp \
	Box2D/Dynamics/b2WorldCallbacks.cpp \
	Box2D/Dynamics/Contacts/b2CircleContact.cpp \
	Box2D/Dynamics/Contacts/b2Contact.cpp \
	Box2D/Dynamics/Contacts/b2ContactSolver.cpp \
	Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
	Box2D/Dynamics/Contacts/b2PolygonContact.cpp \
	Box2D/Dynamics/Contacts/b2TOISolver.cpp \
	Box2D/Dynamics/Joints/b2DistanceJoint.cpp \
	Box2D/Dynamics/Joints/b2FrictionJoint.cpp \
	Box2D/Dynamics/Joints/b2GearJoint.cpp \
	Box2D/Dynamics/Joints/b2Joint.cpp \
	Box2D/Dynamics/Joints/b2LineJoint.cpp \
	Box2D/Dynamics/Joints/b2MouseJoint.cpp \
	Box2D/Dynamics/Joints/b2PrismaticJoint.cpp \
	Box2D/Dynamics/Joints/b2PulleyJoint.cpp \
	Box2D/Dynamics/Joints/b2RevoluteJoint.cpp \
	Box2D/Dynamics/Joints/b2WeldJoint.cpp
	
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_SHARED_LIBRARY)