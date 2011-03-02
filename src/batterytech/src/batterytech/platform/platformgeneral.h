/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : platformgeneral.h
// Description : All public platform function definitions
// Usage       : Include this to make platform calls, see detailed docs
//============================================================================

#ifndef PLATFORMGENERAL_H_
#define PLATFORMGENERAL_H_

// set if the host's memory is in little endian byte order
// at this time, every target arch is little endian.
#define PLATFORM_IS_LITTLE_ENDIAN 1

#include "../audio/AudioManager.h"
#include "../primitives.h"

#ifdef ANDROID_NDK
	#include "android/androidgeneral.h"
#endif /* ANDROID_NDK */

#ifdef _WIN32
	#include "win32/win32general.h"
#endif /* _WIN32 */

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE
	#include "ios/iosgeneral.h"
#endif /* IOS */

#if TARGET_OS_MAC && !TARGET_OS_IPHONE
	#include "osx/osxgeneral.h"
#endif /* MAC */

// must be implemented by target arch
void _platform_log(const char *message);
unsigned char* _platform_load_asset(const char *filename, S32 *size);
void _platform_free_asset(unsigned char *ptr);
S32 _platform_get_asset_length(const char *filename);
S32 _platform_read_asset_chunk(const char *filename, S32 offset, unsigned char *buffer, S32 bufferLength, BOOL32 *eof);
void _platform_get_external_storage_dir_name(char* buf, S32 buflen);
const char* _platform_get_path_separator();
BOOL32 _platform_path_exists(const char* path);
BOOL32 _platform_path_can_read(const char* path);
BOOL32 _platform_path_can_write(const char* path);
BOOL32 _platform_path_create(const char* path);
void _platform_init_sound(AudioManager *audioManager);
void _platform_stop_sound();
void _platform_play_vibration_effect(S32 effectId, F32 intensity);
void _platform_start_vibration_effect(S32 effectId, F32 intensity);
void _platform_stop_vibration_effect(S32 effectId);
void _platform_stop_all_vibration_effects();
BOOL32 _platform_implements_soundpool();
void _platform_init_audiomanagement(S32 streams);
void _platform_release_audiomanagement();
void _platform_load_sound(const char* asset);
S32 _platform_play_sound(const char* asset, F32 leftVol, F32 rightVol, S32 loops, F32 rate);
void _platform_stop_sound(S32 streamId);
void _platform_stop_sound(const char* asset);
void _platform_stop_all_sounds();
void _platform_unload_sound(const char *asset);
void _platform_sound_set_loops(S32 streamId, S32 loops);
void _platform_sound_set_volume(S32 streamId, F32 leftVol, F32 rightVol);
void _platform_sound_set_rate(S32 streamId, F32 rate);
S32 _platform_play_streaming_sound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);
void _platform_stop_streaming_sound(const char *assetName);
void _platform_init_network();
void _platform_release_network();
void _platform_make_non_blocking(SOCKET socket);
S32 _platform_get_socket_last_error();
char** _platform_get_ifaddrs(int *count);
void _platform_free_ifaddrs(char** ifaddrs, int count);
void _platform_show_keyboard();
void _platform_hide_keyboard();
void _platform_exit();
void _platform_show_ad();
void _platform_hide_ad();

// implemented generically
void _platform_convert_path(const char* path, char* newPath);
void _platform_get_basename(const char* path, char *baseName);
void _platform_path_create_recursive(const char* path);
void *get_in_addr(struct sockaddr *sa);

#endif /* PLATFORMGENERAL_H_ */
