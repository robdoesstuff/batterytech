/*
 * androidgeneral.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef ANDROIDGENERAL_CPP_
#define ANDROIDGENERAL_CPP_

#ifdef ANDROID_NDK

#include "../platformgeneral.h"
#include <android/log.h>
#include <jni.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../primitives.h"
#include "../../sound/SoundManager.h"
#include <errno.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>

SoundManager *_andSndMgr;

void _platform_log(const char* message) {
	__android_log_print(ANDROID_LOG_DEBUG, "Imm Pinball", message);
}

unsigned char* _platform_load_asset(const char *filename, S32 *size) {
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

void _platform_get_external_storage_dir_name(char* buf, S32 buflen) {
	__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Getting External Storage Dir");
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	// pull from android java apis
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID loadAssetMethodID = jnienv->GetMethodID(bootClass, "getExternalStorageDirPath", "()Ljava/lang/String;");
	jstring pathStringUTF = (jstring)jnienv->CallObjectMethod(javaBoot, loadAssetMethodID);
	jboolean isCopy;
	const char *jnibuf = jnienv->GetStringUTFChars(pathStringUTF, &isCopy);
	strcpy(buf, jnibuf);
	jnienv->ReleaseStringUTFChars(pathStringUTF, jnibuf);
	__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", buf);
}

const char* _platform_get_path_separator() {
	return "/";
}

BOOL32 _platform_path_exists(const char* path) {
	return (access(path, F_OK) != -1);
}

BOOL32 _platform_path_can_read(const char* path) {
	return (access(path, R_OK) != -1);
}

BOOL32 _platform_path_can_write(const char* path) {
	return (access(path, W_OK) != -1);
}

BOOL32 _platform_path_create(const char* path) {
	return (mkdir(path, (mode_t)0755) == 0);
}

void _platform_init_sound(SoundManager *soundManager) {
	_andSndMgr = soundManager;
}

void _platform_stop_sound() {
	_andSndMgr = NULL;
}

void _platform_play_vibration_effect(S32 effectId, F32 intensity) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "playVibrationEffect", "(IF)V");
	jnienv->CallVoidMethod(javaBoot, methodId, effectId, intensity);
}

void _platform_start_vibration_effect(S32 effectId, F32 intensity) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "startVibrationEffect", "(IF)V");
	jnienv->CallVoidMethod(javaBoot, methodId, effectId, intensity);
}

void _platform_stop_vibration_effect(S32 effectId) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "stopVibrationEffect", "(I)V");
	jnienv->CallVoidMethod(javaBoot, methodId, effectId);
}

void _platform_stop_all_vibration_effects() {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "stopAllVibrationEffects", "()V");
	jnienv->CallVoidMethod(javaBoot, methodId);
}

BOOL32 _platform_implements_soundpool() {
	return TRUE;
}

void _platform_init_soundpool(S32 streams) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "initSoundPool", "(I)V");
	jnienv->CallVoidMethod(javaBoot, methodId, streams);
}

void _platform_release_soundpool() {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "releaseSoundPool", "()V");
	jnienv->CallVoidMethod(javaBoot, methodId);
}

void _platform_load_sound(const char* asset) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "loadSound", "(Ljava/lang/String;)V");
	jstring jAssetname = jnienv->NewStringUTF(asset);
	jnienv->CallVoidMethod(javaBoot, methodId, jAssetname);
}

S32 _platform_play_sound(const char* asset, F32 leftVol, F32 rightVol, S32 loops, F32 rate) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "playSound", "(Ljava/lang/String;FFIF)I");
	jstring jAssetname = jnienv->NewStringUTF(asset);
	jint streamId = jnienv->CallIntMethod(javaBoot, methodId, jAssetname, leftVol, rightVol, loops, rate);
	return streamId;
}

void _platform_stop_sound(S32 streamId) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "stopSound", "(I)V");
	jnienv->CallVoidMethod(javaBoot, methodId, streamId);
}

void _platform_stop_sound(const char* asset) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "stopSound", "(Ljava/lang/String;)V");
	jstring jAssetname = jnienv->NewStringUTF(asset);
	jnienv->CallVoidMethod(javaBoot, methodId, jAssetname);
}

void _platform_stop_all_sounds() {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "stopAllSounds", "()V");
	jnienv->CallVoidMethod(javaBoot, methodId);
}

void _platform_unload_sound(const char *asset) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "unloadSound", "(Ljava/lang/String;)V");
	jstring jAssetname = jnienv->NewStringUTF(asset);
	jnienv->CallVoidMethod(javaBoot, methodId, jAssetname);
}

void _platform_show_keyboard() {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "showKeyboard", "()V");
	jnienv->CallVoidMethod(javaBoot, methodId);
}

void _platform_hide_keyboard() {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "hideKeyboard", "()V");
	jnienv->CallVoidMethod(javaBoot, methodId);
}

void _platform_init_network() {}
void _platform_release_network() {}

void _platform_make_non_blocking(SOCKET socket) {
	fcntl(socket, F_SETFL, O_NONBLOCK);
}

S32 _platform_get_socket_last_error() {
	return errno;
}

char** _platform_get_ifaddrs(int *count) {
	__android_log_print(ANDROID_LOG_DEBUG, "Imm Pinball", "Getting host names");
	*count = 0;
	char** hostnames = NULL;
	struct ifreq *ifr;
	struct ifconf ifc;
	int s, i;
	int numif;

	// find number of interfaces.
	memset(&ifc, 0, sizeof(ifc));
	ifc.ifc_ifcu.ifcu_req = NULL;
	ifc.ifc_len = 0;

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return hostnames;
	}
	if (ioctl(s, SIOCGIFCONF, &ifc) < 0) {
		return hostnames;
	}
	if ((ifr = (struct ifreq*) malloc(ifc.ifc_len)) == NULL) {
		return hostnames;
	}
	ifc.ifc_ifcu.ifcu_req = ifr;
	if (ioctl(s, SIOCGIFCONF, &ifc) < 0) {
		return hostnames;
	}
	close(s);
	numif = ifc.ifc_len / sizeof(struct ifreq);
	*count = numif;
	hostnames = new char*[numif];
	for (i = 0; i < numif; i++) {
		struct ifreq *r = &ifr[i];
		struct sockaddr_in *sin = (struct sockaddr_in *)&r->ifr_addr;
		hostnames[i] = new char[80];
		strcpy(hostnames[i], inet_ntoa(sin->sin_addr));
	}
	free(ifr);
	return hostnames;
}

void _platform_free_ifaddrs(char** ifaddrs, int count) {
	for (S32 i = 0; i < count; i++) {
		delete ifaddrs[i];
	}
	delete [] ifaddrs;
}

#endif /* ANDROID_NDK */

#endif /* ANDROIDGENERAL_CPP_ */
