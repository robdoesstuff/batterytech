/*
 * boot.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifdef ANDROID_NDK
#include "androidtypes.h"
#include "androidgeneral.h"
#include "../../batterytech.h"
#include "importgl.h"
#include <jni.h>
#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif

// the current env for the call
JNIEnv *jnienv;
jobject javaBoot;
extern SoundManager *_andSndMgr; // from androidgeneral

void Java_com_batterypoweredgames_batterytech_Boot_init(JNIEnv* env, jobject thiz, jint width, jint height) {
	jnienv = env;
	javaBoot = thiz;
	btInit(width, height);
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

void Java_com_batterypoweredgames_batterytech_Boot_setPointerState(JNIEnv* env, jobject thiz, jint pointerId, jboolean down, jint x, jint y) {
	jnienv = env;
	javaBoot = thiz;
	btSetPointerState(pointerId, down, x, y);
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

#ifdef __cplusplus
}
#endif

#endif /* _ANDROID */
