/*
 * androidgeneral.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef WIN32GENERAL_CPP_
#define WIN32GENERAL_CPP_

#ifdef ANDROID_NDK

#include "androidgeneral.h"

void _platform_log(const char* message) {
	__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", message);
}

unsigned char* _platform_load_asset(const char *filename, int *size) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	//__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Getting boot class");
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	if (!bootClass) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Boot class not found!");
	}
	//__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Getting method ID");
	jmethodID loadAssetMethodID = jnienv->GetMethodID(bootClass, "loadAsset", "(Ljava/lang/String;)[B");
	if (!loadAssetMethodID) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Method not found!");
	}
	//__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Calling Object Method");
	jstring jfilename = jnienv->NewStringUTF(filename);
	jbyteArray jdata = (jbyteArray)jnienv->CallObjectMethod(javaBoot, loadAssetMethodID, jfilename);
	__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Done Calling Object Method");
	if (!jdata) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "No data returned!");
	}
	// how much data?
	jsize dataLength = jnienv->GetArrayLength(jdata);
	//__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Got data length");
	*size = dataLength;
	//__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Data Size=%d", *size);
	jbyte *data;
	data = (jbyte*) malloc(sizeof(jbyte) * *size);
	if (!data) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "malloc failed!");
	}
	jnienv->GetByteArrayRegion(jdata, 0, *size, data);
	//memcpy(jdata, data, sizeof(unsigned char) * *size);
	return (unsigned char*)data;
}

void _platform_free_asset(unsigned char *ptr) {
	if (ptr) {
		free(ptr);
	}
}

#endif /* ANDROID_NDK */

#endif /* WIN32GENERAL_CPP_ */
