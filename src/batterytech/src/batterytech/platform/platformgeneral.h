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

#if defined(__QNXNTO__)
	#include "blackberry/blackberrygeneral.h"
#endif

#if defined(linux) and not defined(ANDROID_NDK)
	#include "linux/linuxgeneral.h"
#endif

#ifdef CHROME
	#include "chrome/chromegeneral.h"
#endif

//-------------- Files, IO, Paths -------------

/**
 * \ingroup FileIO
 * @{
 */

/** \brief Loads an asset from disk
 *
 * use \ref _platform_free_asset to free the data once done.
 *
 * \param filename The name of the file relative to the assets directory.  Prefix with file: to specify an absolute location in the filesystem.  Prefix with auto: to search external space first but default to internal.
 * \param size output - the size of the asset read
 * \return data as unsigned bytes or \ref NULL if file not found or not readable
 */
unsigned char* _platform_load_asset(const char *filename, S32 *size);

/** \brief Loads an asset from the internal package, bundle or other app-based filesystem
 *
 * use \ref _platform_free_asset to free the data once done.
 *
 * \param filename The name of the file relative to the assets directory.
 * \param size output - the size of the asset read
 * \return data as unsigned bytes or \ref NULL if file not found or not readable
 */
unsigned char* _platform_load_internal_asset(const char *filename, S32 *size);

/** \brief Returns an asset name of either an internal or an absolute location, depending on file existance, settings and input asset name
 *
 * see \ref _platform_load_asset for examples of prefixes
 *
 * batterytech_config.txt also has a setting, asset_default_find_func, which is considered here
 *
 * \param modifiedFilename output - The modified filename (either file:filename for absolute or just filename for internal)
 * \param filename The original asset filename
  */
void _platform_get_modified_asset_name(char *modifiedFilename, const char *filename);

/** \brief Loads a text-based asset from disk
 *
 * Does not strip out LF,CR or other chars.
 * use \ref _platform_free_asset to free the data once done, casting data to unsigned char*
 *
 * \param filename The name of the file relative to the assets directory
 * \return NULL-terminated string or \ref NULL if file not found or not readable
 */
char* _platform_load_text_asset(const char *filename);

/** \brief Frees asset data allocated by \ref _platform_load_asset or \ref _platform_load_text_asset
 * \param ptr The data to free
 */
void _platform_free_asset(unsigned char *ptr);

/** \brief Gets the data length of a file asset
 *
 * \param filename The name of the file relative to the assets directory
 * \return The number of bytes that the file is
 */
S32 _platform_get_asset_length(const char *filename);

/** \brief Reads a portion of an asset from disk
 *
 * starts at offset and ending at EOF or once bufferLength is filled
 *
 * \param filename The name of the file relative to the assets directory
 * \param offset The byte offset to start reading from
 * \param buffer The buffer to read in to
 * \param bufferLength The length of the buffer
 * \param eof output - if EOF has been hit
 * \return number of bytes read
 */
S32 _platform_read_asset_chunk(const char *filename, S32 offset, unsigned char *buffer, S32 bufferLength, BOOL32 *eof);

/** \brief Gets the path of the external storage directory
 *
 * \param buf The output buffer to copy the path into
 * \param buflen The length of the output buffer in bytes
 */
void _platform_get_external_storage_dir_name(char* buf, S32 buflen);

/** \brief Gets the path of the internal (application private) storage directory
 *
 * \param buf The output buffer to copy the path into
 * \param buflen The length of the output buffer in bytes
 */
void _platform_get_application_storage_dir_name(char* buf, S32 buflen);

/** \brief Gets the path separator character(s)
 *
 * \return The backslash, slash or other separator characters
 */
const char* _platform_get_path_separator();

/** \brief Checks for existance of path
 *
 * \param path the absolute path to check
 * \return \ref TRUE if path exists, \ref FALSE if not
 */
BOOL32 _platform_path_exists(const char* path);

/** \brief Checks for readability of path
 *
 * \param path the absolute path to check
 * \return \ref TRUE if path is readable, \ref FALSE if not
 */BOOL32 _platform_path_can_read(const char* path);

/** \brief Checks for writability of path
  *
  * \param path the absolute path to check
  * \return \ref TRUE if path is writable, \ref FALSE if not
  */
BOOL32 _platform_path_can_write(const char* path);

/** \brief Creates a path (if permission allows)
 *
 * \param path The path to create
 * \return \ref TRUE if path created successfully, \ref FALSE if not
 */
BOOL32 _platform_path_create(const char* path);

/** \brief Converts a path into a platform's version of the path
 *
 * \param path The path with any path separator
 * \param newPath output - The path using this platform's separator
 */
void _platform_convert_path(const char* path, char* newPath);

/** \brief Converts a path into UNIX-style forward slash name
 *
 * \param path The path with any path separator
 * \param newPath output - The path using a UNIX forward slash separator
 */
void _platform_convert_path_to_forward(const char* path, char *newPath);

/** \brief Gets the basename of a path
 *
 * Example: /home/files/foo.txt basename is /home/files
 *
 * Note - The path must be using the platform's path separator.  See _platform_convert_path
 *
 * \param path The path to use
 * \param baseName output - The base name of the path
 */
void _platform_get_basename(const char* path, char *baseName);

/** \brief Recursively creates a path (use with caution.)
 *
 * \param path The absolute path to create
 */
void _platform_path_create_recursive(const char* path);

/*@}*/


//-------------- Audio -------------

/**
 * \ingroup Audio
 * @{
 */

/** \brief Initializes platform PCM sound (platform-specific implementation)
 * \param audioManager the \ref BatteryTech::AudioManager
 */
void _platform_init_sound(BatteryTech::AudioManager *audioManager);

/** \brief Stops platform PCM sound */
void _platform_stop_sound();

/** \brief Checks to see if the platform has its own sound fx pool implementation
 * \return \ref TRUE if Sound Pool mechanism supported, \ref FALSE if not */
BOOL32 _platform_implements_soundpool();

/** \brief Initializes the platform's audio management (FX and Streaming)
 * \param streams The number of streams to initialize
 */
void _platform_init_audiomanagement(S32 streams);

/** \brief Releases the platform's audio management (FX and Streaming) */
void _platform_release_audiomanagement();

/** \brief Loads a sound into the platform's audio management (FX)
 * \param asset The sound file asset name
 */
void _platform_load_sound(const char* asset);

/** \brief Plays a sound using the platform's audio management (FX)
 * \param asset The sound file asset name
 * \param leftVol the left channel volume ranging from 0.0 to 1.0
 * \param rightVol the right channel volume ranging from 0.0 to 1.0
 * \param loops the number of loops to play, -1 is forever, 0 is a one-shot sound, 1 will loop just once, etc
 * \param rate playback rate.  1.0 = normal, range is 0.5 to 1.5
 * \return A stream ID used to control the playing sound
 */
S32 _platform_play_sound(const char* asset, F32 leftVol, F32 rightVol, S32 loops, F32 rate);

/** \brief Stops a sound using the platform's audio management (FX)
 * \param streamId the ID of the stream provided by \ref _platform_play_sound
 */
void _platform_stop_sound(S32 streamId);

/** \brief Stops a sound using the platform's audio management (FX)
 * \param asset The name of the file asset to stop
 */
void _platform_stop_sound(const char* asset);

/** \brief Stops all sounds using platform's audio management */
void _platform_stop_all_sounds();

/** \brief Unloads a sound from the platform's audio management (FX)
 * \param asset The name of the file asset to unload
 */
void _platform_unload_sound(const char *asset);

/** \brief Sets the number of loops to play using platform's audio management (FX)
 * \param streamId The ID of the stream provided by \ref _platform_play_sound
 * \param loops the number of loops to play
 */
void _platform_sound_set_loops(S32 streamId, S32 loops);

/** \brief Sets the volume of a stream using platform's audio management (FX)
 * \param streamId The ID of the stream provided by \ref _platform_play_sound
 * \param leftVol the left volume
 * \param rightVol the right volume
 */
void _platform_sound_set_volume(S32 streamId, F32 leftVol, F32 rightVol);

/** \brief Sets the playback rate of a stream using platform's audio management (FX)
 * \param streamId The ID of the stream provided by \ref _platform_play_sound
 * \param rate The new playback rate to use ranging from 0.5 to 1.5
 */
void _platform_sound_set_rate(S32 streamId, F32 rate);

/** \brief Plays a streaming sound (music, long sounds) using the platform's audio management (FX)
 *
 * This uses a different playback mechanism than the standard \ref _platform_play_sound function and is intended for larger files like music.
 * Generally it is implemented using some sort of general media player on the platform and may not support all of the parameters given.
 *
 * \param assetName The name of the file asset
 * \param loops The number of loops to play
 * \param leftVol The left channel volume ranging from 0.0 to 1.0
 * \param rightVol The right channel volume ranging from 0.0 to 1.0
 * \param rate The playback rate ranging from 0.5 to 1.5
 * \return A streaming sound ID - not compatible with standard sound ID
 */
S32 _platform_play_streaming_sound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);

/** \brief Stops a streaming sound (music, long sounds) using the platform's audio management (FX)
 *
 * This will only work if the asset playback was started by \ref _platform_play_streaming_sound
 *
 * \param assetName the name of the asset to stop.
 */
void _platform_stop_streaming_sound(const char *assetName);

/*@}*/


//-------------- Vibration -------------

/**
 * \ingroup Vibration
 * @{
 */

/** \brief Plays a vibration effect
 *
 * \param effectId The effect ID number to play (implementation-specific)
 * \param intensity The intensity to play the effect at (implementation-specific)
 */
void _platform_play_vibration_effect(S32 effectId, F32 intensity);

/** \brief Starts a repeating vibration effect
 *
 * \param effectId The effect ID number to play (implementation-specific)
 * \param intensity The intensity to play the effect at (implementation-specific)
 */
void _platform_start_vibration_effect(S32 effectId, F32 intensity);

/** \brief Stops a repeating vibration effect
 *
 * \param effectId The effect ID number to stop (implementation-specific)
 */
void _platform_stop_vibration_effect(S32 effectId);

/** \brief Stops all vibratoin effects
 *
 */
void _platform_stop_all_vibration_effects();

/*@}*/


//-------------- Networking -------------

/**
 * \ingroup Networking
 * @{
 */

/** \brief Initializes networking for platforms that need it
 *
 * This is called by \ref BatteryTech::NetworkManager but should custom network code be used, this function should always be called before other network
 * functions because it is required by certain platforms such as Windows to initialize sockets before they can be used.
 */
void _platform_init_network();

/** \brief Releases networking for platforms that need it
 *
 * This is called by \ref BatteryTech::NetworkManager bu should custom network code be used, this function should always be called before application shutdown
 * to release networking resources for certain platforms such as Windows.
 */
void _platform_release_network();

/** \brief Makes a socket non-blocking
 *
 * \param socket The socket to make non-blocking
 */
void _platform_make_non_blocking(SOCKET socket);

/** \brief Gets the last socket error state
 *
 * \return An error code or EWOULDBLOCK if socket is non-blocking
 */
S32 _platform_get_socket_last_error();

/** \brief Gets a list of network interface addresses
 *
 * You must free the list returned by \ref _platform_free_ifaddrs when done
 *
 * \param count OUT - number of ifaddrs
 * \return a list of socket interface addresses
 */
char** _platform_get_ifaddrs(int *count);

/** \brief Frees the list of interface addresses acquired by _platform_get_ifaddrs
 *
 * \param ifaddrs The list of ifaddrs acquired by \ref _platform_get_ifaddrs
 * \param count The count set by \ref _platform_get_ifaddrs
 */
void _platform_free_ifaddrs(char** ifaddrs, int count);

/** \brief Gets the internet address of a sockaddr structure
 *
 * A sockaddr structure is used throughout standard BSD sockets but translating to an internet address is platform-specific.
 *
 * Here is an example to find the hostname from a packet received via udp:
 * \code
 *  struct sockaddr_storage their_addr;
 *  recvfrom(udpListenerSocket, buf, UDPBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len))
 *	const char *hostname = inet_ntop(their_addr.ss_family,
 *		get_in_addr((struct sockaddr *)&their_addr),
 *		s, sizeof s);
 * \endcode
 *
 * \param sa The socket address to look up
 * \return The internet address of the socket
 */
void *get_in_addr(struct sockaddr *sa);


/*@}*/

//--------------- Keyboard ---------------

/**
 * \ingroup Keyboard
 * @{
 */

/** \brief Shows the soft keyboard
 *
 * If no virtual keyboard exists, nothing will be shown.
 *
 */
void _platform_show_keyboard();

/** \brief Hides the soft keyboard
 *
 */
void _platform_hide_keyboard();

/** \brief Checks for existance of special key
 *
 * The special key correlates with the special key in \ref BatteryTech::Context::specialKeyPressed
 *
 * \param sKey The special key to check for
 * \returns \ref TRUE if special key is on this platform, \ref FALSE if not
 */
BOOL32 _platform_has_special_key(BatteryTech::SpecialKey sKey);

/*@}*/

//-------------- Other --------------
/**
 * \ingroup OtherPlatform
 * @{
 */

/** \brief Exits the application (where applicable)
 *
 * Many platforms don't support this
 */
void _platform_exit();


/** \brief Logs a message
 * In combination with sprintf, it's simple to construct log messages with no memory allocations
 *
 * Example:
 * \code
 * buf[1024]; // large enough to ensure no overflow
 * sprintf(buf, "This is a log message involving %d and %f", i, j);
 * logmsg(buf);
 * \endcode
 */
void _platform_log(const char *message);

/** \brief Returns a time in nanoseconds, suitable for high resolution timing and profiling
 *
 * This function is designed specifically for profiling
 *
 * Example:
 * \code
 * U64 startTime = _platform_get_time_nanos();
 *  // run some code
 * U64 endTime = _platform_get_time_nanos();
 * buf[200];
 * sprintf(buf, "Function took %u ns", (endTime - startTime));
 * logmsg(buf);
 * \endcode
 * \return time in nanoseconds
 */
U64 _platform_get_time_nanos();

/**@}*/

/**
 * \ingroup Hooks
 * @{
 */

/** \brief Shows an ad (requires platform-specific and ad-sdk support)
 *
 */
void _platform_show_ad();

/** \brief Hides an ad (requires platform-specific and ad-sdk support)
 *
 */
void _platform_hide_ad();

/** \brief Any platform customizations can be done through this hook inteface.
 *
 * All hooks require code on the platform.
 *
 * \param hook The hook data
 * \param result The result (if the hook is synchronous, otherwise Context::callbackData will hold the result)
 * \param resultLen The length of the result buffer
 *
 * Example:
 * \code
 * char resultBuffer[10];
 * _platform_hook("hasFrontCamera", resultBuffer, 10);
 * if (strEquals(resultBuffer, "true")) {
 *     logmsg("has front camera");
 * }
 * \endcode
 */
void _platform_hook(const char *hook, char *result, S32 resultLen);
/**@}*/

#endif /* PLATFORMGENERAL_H_ */
