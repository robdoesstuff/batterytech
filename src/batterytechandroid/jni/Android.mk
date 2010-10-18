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
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL

LOCAL_MODULE    := batterytech
LOCAL_SRC_FILES :=\
	src/batterytech/batterytech.cpp \
	src/batterytech/Logger.cpp \
	src/batterytech/Context.cpp \
	src/batterytech/decoders/stb_image.c \
	src/batterytech/decoders/stb_vorbis.c \
	src/batterytech/sound/PCMSound.cpp \
	src/batterytech/sound/PCMStream.cpp \
	src/batterytech/sound/SoundManager.cpp \
	src/batterytech/render/Renderer.cpp \
	src/batterytech/render/TextRasterRenderer.cpp \
	src/batterytech/render/WorldRenderer.cpp \
	src/batterytech/render/GraphicsConfiguration.cpp \
	src/batterytech/render/MenuRenderer.cpp \
	src/batterytech/platform/android/androidgeneral.cpp \
	src/batterytech/platform/android/boot.cpp \
	src/batterytech/platform/android/importgl.c \
	src/batterytech/platform/opengles.cpp \
	src/batterytech/game/World.cpp \
	src/batterytech/game/Game.cpp \
	src/batterytech/ui/Button.cpp \
	src/batterytech/ui/Checkbox.cpp \
	src/batterytech/ui/Label.cpp \
	src/batterytech/ui/Layout.cpp \
	src/batterytech/ui/LinearLayout.cpp \
	src/batterytech/ui/Menu.cpp \
	src/batterytech/ui/UIComponent.cpp \
	src/batterytech/ui/UIManager.cpp \
	src/batterytech/ui/UIAnimator.cpp \
	src/batterytech/ui/SlideAnimator.cpp \
	src/batterytech/menus/MainMenu.cpp \
	src/batterytech/menus/OptionsMenu.cpp
	
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_SHARED_LIBRARY)