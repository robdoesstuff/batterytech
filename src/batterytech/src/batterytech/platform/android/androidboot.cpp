/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : boot.cpp
// Description : Android bootstrap and JNI linkage
//============================================================================

#ifdef ANDROID_NDK
#include "androidtypes.h"
#include "../platformgeneral.h"
#include "../../batterytech.h"
#include "../../render/GraphicsConfiguration.h"
#include "importgl.h"
#include <jni.h>
#include <android/log.h>
#include "../../util/strx.h"
#include "../opengles.h"
#include "../../audio/AudioManager.h"

using namespace BatteryTech;

#ifdef __cplusplus
extern "C" {
#endif

// the current env for the call
JNIEnv *jnienv;
jobject javaBoot;
extern AudioManager *_andSndMgr; // from androidgeneral
GraphicsConfiguration *gConfig;

void determineGPUCapabilities();
BatteryTech::SpecialKey getSpecialKey(int keyCode);

void Java_com_batterypoweredgames_batterytech_Boot_init(JNIEnv* env, jobject thiz, jint width, jint height, jboolean usingGLES2) {
	jnienv = env;
	javaBoot = thiz;
	importGLInit(usingGLES2);
	gConfig = new GraphicsConfiguration;
	determineGPUCapabilities();
	btInit(gConfig, width, height);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_setScreenSize(JNIEnv* env, jobject thiz, jint width, jint height) {
	jnienv = env;
	javaBoot = thiz;
	btSetScreenSize(width, height);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_release(JNIEnv* env, jobject thiz) {
	jnienv = env;
	javaBoot = thiz;
	btRelease();
	delete gConfig;
	importGLDeinit();
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_update(JNIEnv* env, jobject thiz, jfloat delta) {
	//__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech Boot", "delta is %f", delta);
	jnienv = env;
	javaBoot = thiz;
	btUpdate((F32)delta);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_draw(JNIEnv* env, jobject thiz) {
	jnienv = env;
	javaBoot = thiz;
	btDraw();
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_suspend(JNIEnv* env, jobject thiz) {
	jnienv = env;
	javaBoot = thiz;
	btSuspend();
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_resume(JNIEnv* env, jobject thiz) {
	jnienv = env;
	javaBoot = thiz;
	btResume();
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_setPointerState(JNIEnv* env, jobject thiz, jint pointerId, jboolean down, jint x, jint y) {
	jnienv = env;
	javaBoot = thiz;
	btSetPointerState(pointerId, down, x, y);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_keyUp(JNIEnv* env, jobject thiz, jint keyChar, jint keyCode) {
	jnienv = env;
	javaBoot = thiz;
	btKeyUp(keyChar, getSpecialKey(keyCode));
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_keyDown(JNIEnv* env, jobject thiz, jint keyChar, jint keyCode) {
	jnienv = env;
	javaBoot = thiz;
	btKeyDown(keyChar, getSpecialKey(keyCode));
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_keyPressed(JNIEnv* env, jobject thiz, jint keyChar, jint keyCode) {
	jnienv = env;
	javaBoot = thiz;
	btKeyPressed(keyChar, getSpecialKey(keyCode));
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_accelerometerChanged(JNIEnv* env, jobject thiz, jfloat x, jfloat y, jfloat z) {
	//__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech Boot", "delta is %f", delta);
	jnienv = env;
	javaBoot = thiz;
	btAccelerometerChanged(x,y,z);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_fillAudioBuffer(JNIEnv* env, jobject thiz, jshortArray jPCMData, jint length) {
	if (_andSndMgr) {
		signed short *buf = new signed short[length / 2];
		_andSndMgr->fillBuffer(buf, length);
		env->SetShortArrayRegion(jPCMData, 0, length / 2, buf);
		delete [] buf;
	}
}

jboolean Java_com_batterypoweredgames_batterytech_Boot_callback(JNIEnv* env, jobject thiz, jstring data) {
	jnienv = env;
	javaBoot = thiz;
	jboolean ret = FALSE;
	if (data) {
		jboolean isCopy;
		const char *jnibuf = jnienv->GetStringUTFChars(data, &isCopy);
		ret = btCallback(jnibuf);
		jnienv->ReleaseStringUTFChars(data, jnibuf);
	}
	jnienv = 0;
	javaBoot = 0;
	return ret;
}

void determineGPUCapabilities() {
	const char *vendor = (const char*)glGetString(GL_VENDOR);
	const char *renderer = (const char*)glGetString(GL_RENDERER);
	const char *version = (const char*)glGetString(GL_VERSION);
	const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
	// 1.0 = 0, 1.1 = 1, 2.0/3.0 = 2
	int ver = 0;
	__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL Vendor %s", vendor);
	__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL Renderer %s", renderer);
	__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL Version %s", version);
	if ((strstr(version, "1.0") || strstr(version, "1.1")) && !(strstr(version, "2.0") || strstr(version, "3.0"))) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL ES Version 1.0/1.1 found");
		if (!strstr(version, "1.0")) {
			ver = 1;
		}
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL ES Version 2.0/3.0 found");
		gConfig->supportsShaders = TRUE;
		gConfig->supportsFBOs = TRUE;
		ver = 2;
	}
	if ((ver > 0) || gles_checkExtension(GLES_EXT_GENERATE_MIPMAP)) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "HW MipMap Gen Supported");
		gConfig->supportsHWmipmapgen = TRUE;
	}
	if (gles_checkExtension(GLES_EXT_TEXTURE_FLOAT)) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Float textures supported");
		gConfig->supportsFloatTextures = TRUE;
	}
	if (gles_checkExtension(GLES_EXT_DEPTH_TEXTURE)) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Depth textures supported");
		gConfig->supportsDepthTextures = TRUE;
	}
	if ((ver > 0) || gles_checkExtension(GLES_EXT_VERTEX_BUFFER_OBJECT)) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "VBOs Supported");
		gConfig->supportsVBOs = TRUE;
	}
	if (strstr(renderer, "MSM7500")) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "HW UV Transform not supported");
		gConfig->supportsUVTransform = FALSE;
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "HW UV Transform supported");
		gConfig->supportsUVTransform = TRUE;
	}
	if (strstr(renderer, ANDROID_RENDERER_PIXELFLINGER)) {
		gConfig->softGPU = TRUE;
	}
}

BatteryTech::SpecialKey getSpecialKey(int keyCode) {
	if (keyCode == 82) {
		return BatteryTech::SKEY_MENU;
	} else if (keyCode == 4) {
		return BatteryTech::SKEY_BACK;
	} else if (keyCode == 27) {
		return BatteryTech::SKEY_CAMERA;
	} else {
		return BatteryTech::SKEY_NULL;
	}
}

void Java_com_batterypoweredgames_batterytech_Boot_setGraphicsContextLost(JNIEnv* env, jobject thiz, jboolean wasLost) {
	jnienv = env;
	javaBoot = thiz;
	btSetGraphicsContextLost(wasLost);
	jnienv = 0;
	javaBoot = 0;
}
#ifdef __cplusplus
}
#endif


#endif /* _ANDROID */
