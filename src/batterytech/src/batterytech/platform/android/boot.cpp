/*
 * boot.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

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
#include "../../sound/SoundManager.h"

#ifdef __cplusplus
extern "C" {
#endif

// the current env for the call
JNIEnv *jnienv;
jobject javaBoot;
extern SoundManager *_andSndMgr; // from androidgeneral
GraphicsConfiguration *gConfig;

void determineGPUCapabilities();

void Java_com_batterypoweredgames_batterytech_Boot_init(JNIEnv* env, jobject thiz, jint width, jint height) {
	jnienv = env;
	javaBoot = thiz;
	gConfig = new GraphicsConfiguration;
	determineGPUCapabilities();
	btInit(gConfig, width, height);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_release(JNIEnv* env, jobject thiz) {
	jnienv = env;
	javaBoot = thiz;
	btRelease();
	delete gConfig;
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

void Java_com_batterypoweredgames_batterytech_Boot_keyUp(JNIEnv* env, jobject thiz, jint keyCode) {
	jnienv = env;
	javaBoot = thiz;
	btKeyUp(keyCode);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_keyDown(JNIEnv* env, jobject thiz, jint keyCode) {
	jnienv = env;
	javaBoot = thiz;
	btKeyDown(keyCode);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_keyPressed(JNIEnv* env, jobject thiz, jint keyCode) {
	jnienv = env;
	javaBoot = thiz;
	btKeyPressed(keyCode);
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

void determineGPUCapabilities() {
	const char *vendor = (const char*)glGetString(GL_VENDOR);
	const char *renderer = (const char*)glGetString(GL_RENDERER);
	const char *version = (const char*)glGetString(GL_VERSION);
	const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
	// 1.0 = 0, 1.1 = 1, 2.0 = 2
	int ver = 0;
	if (strStartsWith(version, GLES_VERSION_CONSTANT)) {
		if (strstr(version, "2.0")) {
			__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL ES Version 2.0 found");
			ver = 2;
		} else if (strstr(version, "1.1")) {
			__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL ES Version 1.1 found");
			ver = 1;
		}
	}
	if (ver == 0) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL ES Version 1.0 found");
	}
	if ((ver > 0) || gles_checkExtension(GLES_EXT_GENERATE_MIPMAP)) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "HW MipMap Gen Supported");
		gConfig->supportsHWmipmapgen = TRUE;
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

#ifdef __cplusplus
}
#endif

#endif /* _ANDROID */
