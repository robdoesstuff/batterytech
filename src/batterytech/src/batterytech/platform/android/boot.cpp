/*
 * boot.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifdef ANDROID_NDK
#include "../../batterytech.h"
#include "importgl.h"
#include <jni.h>
#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif
void Java_com_batterypoweredgames_batterytech_Boot_init(JNIEnv* env, jobject thiz) {
	 btInit();
}

void Java_com_batterypoweredgames_batterytech_Boot_update(JNIEnv* env, jobject thiz, jfloat delta) {
	//__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech Boot", "delta is %f", delta);
	btUpdate((F32)delta);
}

void Java_com_batterypoweredgames_batterytech_Boot_draw(JNIEnv* env, jobject thiz) {
	 btDraw();
}
#ifdef __cplusplus
}
#endif

#endif /* _ANDROID */
