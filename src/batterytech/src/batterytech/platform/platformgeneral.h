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


//-------------- Files, IO, Paths -------------

// Loads an asset, output is unsigned char, use _platform_free_asset to free it once done.  Asset size will be set as argument *size
unsigned char* _platform_load_asset(const char *filename, S32 *size);

// Loads a text-based asset.  Does not strip out LF,CR or other chars.  Returns null-terminated string.
char* _platform_load_text_asset(const char *filename);

// Frees an asset allocated by _platform_load_asset
void _platform_free_asset(unsigned char *ptr);

// Gets the length of an asset
S32 _platform_get_asset_length(const char *filename);

// Reads a portion of an asset into *buffer, starting at offset and ending at EOF or once bufferLength is filled
S32 _platform_read_asset_chunk(const char *filename, S32 offset, unsigned char *buffer, S32 bufferLength, BOOL32 *eof);

// Gets the path of the external storage directory
void _platform_get_external_storage_dir_name(char* buf, S32 buflen);

// Gets the path of the internal (application private) storage directory
void _platform_get_application_storage_dir_name(char* buf, S32 buflen);

// Gets the path separator character(s)
const char* _platform_get_path_separator();

// Checks for existance of path
BOOL32 _platform_path_exists(const char* path);

// Checks for readability of path
BOOL32 _platform_path_can_read(const char* path);

// Checks for writability of path
BOOL32 _platform_path_can_write(const char* path);

// Creates a path (if permission allows)
BOOL32 _platform_path_create(const char* path);

// Converts a path into a platform's version of the path
void _platform_convert_path(const char* path, char* newPath);

// COnverts a path into UNIX-style forward slash name
void _platform_convert_path_to_forward(const char* path, char *newPath);

// Gets the basename of a path (such as /home/files/foo.txt basename is /home/files)
void _platform_get_basename(const char* path, char *baseName);

// Recursively creates a path (use with caution.)
void _platform_path_create_recursive(const char* path);


//-------------- Audio -------------

// Initializes platform PCM sound (platform-specific implementation)
void _platform_init_sound(BatteryTech::AudioManager *audioManager);

// Stops platform PCM sound
void _platform_stop_sound();

// Checks to see if the platform has its own sound fx pool implementation
BOOL32 _platform_implements_soundpool();

// Initializes the platform's audio management (FX and Streaming)
void _platform_init_audiomanagement(S32 streams);

// Releases the platform's audio management (FX and Streaming)
void _platform_release_audiomanagement();

// Loads a sound into the platform's audio management (FX)
void _platform_load_sound(const char* asset);

// Plays a sound using the platform's audio management (FX)
S32 _platform_play_sound(const char* asset, F32 leftVol, F32 rightVol, S32 loops, F32 rate);

// Stops a sound using the platform's audio management (FX)
void _platform_stop_sound(S32 streamId);

// Stops a sound using the platform's audio management (FX)
void _platform_stop_sound(const char* asset);

// Stops all sounds using platform's audio management
void _platform_stop_all_sounds();

// Unloads a sound from the platform's audio management (FX)
void _platform_unload_sound(const char *asset);

// Sets the number of loops to play using platform's audio management (FX)
void _platform_sound_set_loops(S32 streamId, S32 loops);

// Sets the volume of a stream using platform's audio management (FX)
void _platform_sound_set_volume(S32 streamId, F32 leftVol, F32 rightVol);

// Sets the playback rate of a stream using platform's audio management (FX)
void _platform_sound_set_rate(S32 streamId, F32 rate);

// Plays a streaming sound (music, long sounds) using the platform's audio management (FX)
S32 _platform_play_streaming_sound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);

// Stops a streaming sound (music, long sounds) using the platform's audio management (FX)
void _platform_stop_streaming_sound(const char *assetName);


//-------------- Vibration -------------

// Plays a vibration effect (effect implementation is platform-specific)
void _platform_play_vibration_effect(S32 effectId, F32 intensity);

// Starts a repeating vibration effect (effect implementation is platform-specific)
void _platform_start_vibration_effect(S32 effectId, F32 intensity);

// Stops a repeating vibration effect (effect implementation is platform-specific)
void _platform_stop_vibration_effect(S32 effectId);

// Stops all vibration effects (effect implementation is platform-specific)
void _platform_stop_all_vibration_effects();


//-------------- Networking -------------

// Initializes networking for platforms that need it
void _platform_init_network();

// Releases networking for platforms that need it
void _platform_release_network();

// Makes a socket non-blocking
void _platform_make_non_blocking(SOCKET socket);

// Gets the last socket error state
S32 _platform_get_socket_last_error();

// Gets a list of network interface addresses, returning number of which into count
char** _platform_get_ifaddrs(int *count);

// Frees the list of interface addresses acquired by _platform_get_ifaddrs
void _platform_free_ifaddrs(char** ifaddrs, int count);

// Gets the internet address of a sockaddr structure
void *get_in_addr(struct sockaddr *sa);

//--------------- Keyboard ---------------

// Shows the soft keyboard
void _platform_show_keyboard();

// Hides the soft keyboard
void _platform_hide_keyboard();

// Checks for existance of special key
BOOL32 _platform_has_special_key(BatteryTech::SpecialKey sKey);


//-------------- Other --------------

// Exits the application
void _platform_exit();

// Shows an ad (requires platform-specific and ad-sdk support)
void _platform_show_ad();

// Hides an ad (requires platform-specific and ad-sdk support)
void _platform_hide_ad();

// Any platform customizations can be done through this hook inteface.  All hooks require code on the platform.
void _platform_hook(const char *hook, char *result, S32 resultLen);

// Logs a message
void _platform_log(const char *message);

// Returns a time in nanoseconds, suitable for high resolution timing and profiling
U64 _platform_get_time_nanos();



#endif /* PLATFORMGENERAL_H_ */
