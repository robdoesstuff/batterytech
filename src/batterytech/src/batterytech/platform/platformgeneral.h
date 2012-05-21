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

/**
 * \defgroup FileIO File Asset Reading and Writing
 * \brief File Assets, File IO, Directories and Paths
 *
 * Reading may happen from an Android APK, iOS/OSX Bundle or a plain old Windows, Linux or other file system.  Many of the supported input systems
 * can not be written to so it is recommended that an application developer always plan on writing to external or application storage (whichever is more
 * appropriate) instead of ever assuming that the input files may ever be written to.
 *
 * Loading and freeing an asset example:
 * \code
 * S32 size;
 * unsigned char* data = _platform_load_asset("myasset.dat", &size);
 * if (data) {
 *     // process data of size
 * }
 * _platform_free_asset(data);
 * \endcode
 * @{
 */

//-------------- Files, IO, Paths -------------

/** \brief Loads an asset from disk
 *
 * use \ref _platform_free_asset to free the data once done.
 *
 * \param filename The name of the file relative to the assets directory
 * \param size output - the size of the asset read
 * \return data as unsigned bytes or \ref NULL if file not found or not readable
 */
unsigned char* _platform_load_asset(const char *filename, S32 *size);

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

/**
 * \defgroup Audio Audio playback and management
 * \brief Audio Sample and (local) Streaming playback
 *
 * BatteryTech supports OGG-Vorbis exclusively.
 *
 * _platform audio functions are available primarily to serve the \ref BatteryTech::AudioManager.
 * For most applications, \ref BatteryTech::AudioManager is the desired interface to use and can
 * be accessed through the application's \ref BatteryTech::Context.
 *
 * Example:
 * \code
 * context->audioManager->loadSound("sounds/mysound.ogg");
 * context->audioManager->playSound("sounds/mysound.ogg", 0, 1.0, 1.0, 1.0);
 * context->audioManager->unloadSound("sounds/mysound.ogg");
 * \endcode
 *
 * @{
 */

//-------------- Audio -------------

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

/**
 * \defgroup Vibration Vibration Control
 * \brief Support for vibration effect playback
 *
 * Most systems do not have robust vibration control, but specifically on Android, there are systems such as Immersion Technology's Motiv or UHL
 * which can play a variety of effects at various intensities.  Such systems will need to be integrated on the Android platform side.  Stock Android
 * vibration support can also be integrated instead.  Please follow the instructions in the section on Integrating platform-specific systems for
 * additional information.
 *
 * @{
 */

//-------------- Vibration -------------

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

/**
 * \defgroup Networking Networking
 * \brief POSIX Socket-based network support
 *
 * Most platforms natively support standard BSD or POSIX sockets but for those that don't, simply including \ref platformgeneral.h will include, define or redefine
 * macros to support POSIX.  Applications can count on standard functions such as socket, bind, setsockopt, send, recv, inet_pton, inet_ntop, getaddrinfo and getnameinfo
 *
 * A few specific functions that are not defined as part of the POSIX sockets specification are implemented as _platform functions and documented here.
 *
 * For more information on using Berkeley/POSIX sockets, please visit the wikipedia link in the see also section.
 *
 * BatteryTech includes a default implementation of message-based client/host game networking in the following classes:
 * \ref BatteryTech::NetworkManager, \ref BatteryTech::GameConnection, \ref BatteryTech::NetworkMessage
 *
 * \see http://en.wikipedia.org/wiki/Berkeley_socket
 * @{
 */

//-------------- Networking -------------

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

/**
 * \defgroup Keyboard Keyboard
 * @{
 */

//--------------- Keyboard ---------------

// Shows the soft keyboard
void _platform_show_keyboard();

// Hides the soft keyboard
void _platform_hide_keyboard();

// Checks for existance of special key
BOOL32 _platform_has_special_key(BatteryTech::SpecialKey sKey);


/*@}*/

/**
 * \defgroup OtherPlatform Other Platform Functions
 * @{
 */
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

/**@}*/


#endif /* PLATFORMGENERAL_H_ */
