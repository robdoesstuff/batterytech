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
// Name        : androidgeneral.cpp
// Description : Android platform general functions
// Usage       : See platformgeneral.h for usage
//============================================================================

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
#include "../../audio/AudioManager.h"
#include <errno.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include "../../Context.h"
#include "../../batterytech.h"

using namespace BatteryTech;

AudioManager *_andSndMgr;

void _platform_log(const char* message) {
	__android_log_print(ANDROID_LOG_DEBUG, "Batterytech App", message);
}

unsigned char* _platform_load_internal_asset(const char *filename, S32 *size) {
	// __android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Loading asset %s", filename);
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID loadAssetMethodID = jnienv->GetMethodID(bootClass, "loadAsset", "(Ljava/lang/String;)[B");
	jstring jfilename = jnienv->NewStringUTF(filename);
	jbyteArray jdata = (jbyteArray)jnienv->CallObjectMethod(javaBoot, loadAssetMethodID, jfilename);
	jnienv->DeleteLocalRef(bootClass);
	jnienv->DeleteLocalRef(jfilename);
	if (!jdata) {
		return NULL;
	}
	// how much data?
	jsize dataLength = jnienv->GetArrayLength(jdata);
	//__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Got data length");
	*size = dataLength;
	// __android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Data Size=%d", *size);
	jbyte *data;
	data = (jbyte*) malloc(sizeof(jbyte) * *size);
	if (!data) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "malloc failed!");
		return NULL;
	}
	jnienv->GetByteArrayRegion(jdata, 0, *size, data);
	//memcpy(jdata, data, sizeof(unsigned char) * *size);
	jnienv->DeleteLocalRef(jdata);
	return (unsigned char*)data;
}

void _platform_free_asset(unsigned char *ptr) {
	if (ptr) {
		free(ptr);
	}
}

S32 _platform_get_asset_length(const char *filename) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID loadAssetMethodID = jnienv->GetMethodID(bootClass, "getAssetLength", "(Ljava/lang/String;)I");
	jstring jfilename = jnienv->NewStringUTF(filename);
	jint dataLength = jnienv->CallIntMethod(javaBoot, loadAssetMethodID, jfilename);
	return dataLength;
}

S32 _platform_read_asset_chunk(const char *filename, S32 offset, unsigned char *buffer, S32 bufferLength, BOOL32 *eof) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID loadAssetMethodID = jnienv->GetMethodID(bootClass, "readAssetChunk", "(Ljava/lang/String;II)[B");
	jstring jfilename = jnienv->NewStringUTF(filename);
	jbyteArray jdata = (jbyteArray)jnienv->CallObjectMethod(javaBoot, loadAssetMethodID, jfilename, offset, bufferLength);
	if (!jdata) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "No data returned!");
	}
	jsize dataLength = jnienv->GetArrayLength(jdata);
	jnienv->GetByteArrayRegion(jdata, 0, dataLength, (jbyte*)buffer);
	*eof = (dataLength < bufferLength);
	return dataLength;
}


void _platform_get_application_storage_dir_name(char* buf, S32 buflen) {
	// __android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Getting Application Storage Dir");
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	// pull from android java apis
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID loadAssetMethodID = jnienv->GetMethodID(bootClass, "getApplicationStorageDirPath", "()Ljava/lang/String;");
	jstring pathStringUTF = (jstring)jnienv->CallObjectMethod(javaBoot, loadAssetMethodID);
	jboolean isCopy;
	const char *jnibuf = jnienv->GetStringUTFChars(pathStringUTF, &isCopy);
	strcpy(buf, jnibuf);
	jnienv->ReleaseStringUTFChars(pathStringUTF, jnibuf);
	// __android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", buf);
}

void _platform_get_external_storage_dir_name(char* buf, S32 buflen) {
	// __android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "Getting External Storage Dir");
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
	strcat(buf, "/");
	strcat(buf, btGetContext()->appProperties->get("storage_dir")->getValue());
	// __android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", buf);
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

void _platform_init_sound(AudioManager *audioManager) {
	_andSndMgr = audioManager;
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "startAudioTrack", "()V");
	jnienv->CallVoidMethod(javaBoot, methodId);
}

void _platform_stop_sound() {
	_andSndMgr = NULL;
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "stopAudioTrack", "()V");
	jnienv->CallVoidMethod(javaBoot, methodId);
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

void _platform_init_audiomanagement(S32 streams) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "initAudioManagement", "(I)V");
	jnienv->CallVoidMethod(javaBoot, methodId, streams);
}

void _platform_release_audiomanagement() {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "releaseAudioManagement", "()V");
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

void _platform_sound_set_loops(S32 streamId, S32 loops) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "setSoundLoops", "(II)V");
	jnienv->CallVoidMethod(javaBoot, methodId, streamId, loops);
}

void _platform_sound_set_volume(S32 streamId, F32 leftVol, F32 rightVol) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "setSoundVolume", "(IFF)V");
	jnienv->CallVoidMethod(javaBoot, methodId, streamId, leftVol, rightVol);
}

void _platform_sound_set_rate(S32 streamId, F32 rate) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "setSoundRate", "(IF)V");
	jnienv->CallVoidMethod(javaBoot, methodId, streamId, rate);
}

S32 _platform_play_streaming_sound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "playStreamingSound", "(Ljava/lang/String;FFIF)I");
	jstring jAssetname = jnienv->NewStringUTF(assetName);
	jint streamId = jnienv->CallIntMethod(javaBoot, methodId, jAssetname, leftVol, rightVol, loops, rate);
	return streamId;
}

void _platform_stop_streaming_sound(const char *assetName) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "stopStreamingSound", "(Ljava/lang/String;)V");
	jstring jAssetname = jnienv->NewStringUTF(assetName);
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

void _platform_exit() {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "exit", "()V");
	jnienv->CallVoidMethod(javaBoot, methodId);
}

void _platform_show_ad() {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "showAd", "()V");
	jnienv->CallVoidMethod(javaBoot, methodId);
}

void _platform_hide_ad() {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID methodId = jnienv->GetMethodID(bootClass, "hideAd", "()V");
	jnienv->CallVoidMethod(javaBoot, methodId);
}

BOOL32 _platform_has_special_key(BatteryTech::SpecialKey sKey) {
	if (sKey == BatteryTech::SKEY_BACK || sKey == BatteryTech::SKEY_MENU) {
		return TRUE;
	}
	return FALSE;
}

void _platform_hook(const char *hook, char *result, S32 resultLen) {
	extern JNIEnv* jnienv;
	extern jobject javaBoot;
	// pull from android java apis
	jclass bootClass = jnienv->GetObjectClass(javaBoot);
	jmethodID loadAssetMethodID = jnienv->GetMethodID(bootClass, "hook", "(Ljava/lang/String;)Ljava/lang/String;");
	jstring jhook = jnienv->NewStringUTF(hook);
	jstring resultStringUTF = (jstring)jnienv->CallObjectMethod(javaBoot, loadAssetMethodID, jhook);
	// only copy the result from Java if we've got somewhere to put it
	if (result && resultLen > 0) {
		jboolean isCopy;
		const char *jnibuf = jnienv->GetStringUTFChars(resultStringUTF, &isCopy);
		if (jnibuf) {
			strncpy(result, jnibuf, resultLen);
			result[resultLen-1] = '\0';
		} else {
			result[0] = '\0';
		}
		jnienv->ReleaseStringUTFChars(resultStringUTF, jnibuf);
	}
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
	__android_log_print(ANDROID_LOG_DEBUG, "Batterytech App", "Getting host names");
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

// Returns a time in nanoseconds, suitable for high resolution timing and profiling
U64 _platform_get_time_nanos() {
   timespec ts;
   clock_gettime(CLOCK_MONOTONIC, &ts);
   return (uint64_t)ts.tv_sec * 1000000000LL + (uint64_t)ts.tv_nsec;
}

#endif /* ANDROID_NDK */

#endif /* ANDROIDGENERAL_CPP_ */
