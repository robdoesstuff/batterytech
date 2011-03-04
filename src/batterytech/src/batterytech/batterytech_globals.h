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
// Name        : batterytech_globals.h
// Description : Global Settings - Change as needed!
//============================================================================

#ifndef BATTERYTECH_GLOBALS_H_
#define BATTERYTECH_GLOBALS_H_

#include "primitives.h"

/// ---------------- General -----------------
// Are we in debug mode? (this is currently unused in batterytech)
#define BT_DEBUG FALSE
// Will you support a GLES 2.0 shader pipeline if the device supports it?
#define USE_SHADERS_WHEN_SUPPORTED TRUE
// What width are you designing your UI to?  This will become your UI width unit.
#define REFERENCE_WIDTH 480
// What height are you designing your UI to?  This will become your UI height unit.
#define REFERENCE_HEIGHT 800
// Number of samples to average for use as the tick delta in updates
// Many Android devices have erratic updates without smoothing.
#define TICK_SMOOTHER_SAMPLES 15
// Maximum number of touch/pointers we support
#define MAX_POINTERS 10

/// -------------- Windows -------------------
// If a console window is available, display it?
#define CONSOLE_LOG_ENABLED_WHEN_AVAILABLE TRUE
// If your app were a test window, what would the width be?
#define WINDOW_WIDTH 480
// If your app were a test window, what would the height be?
#define WINDOW_HEIGHT 800
// If your app were a test window, what would the title be?
#define WINDOWED_APP_NAME "BatteryTech Demo App"

/// -------------- UI and Text --------------
// When rasterizing a font to a texture, this is the first width attempted.
#define INITIAL_FONT_TEXTURE_WIDTH 256
// When rasterizing a font to a texture, this is the first height attempted
#define INITIAL_FONT_TEXTURE_HEIGHT 256
// Maximum number of components you can add to any other UI component (including layouts).
// Increase if necessary.
#define MAX_UI_SUBCOMPONENTS 10

/// -------------- audio --------------------
// Would you like to use the platform's audio management (soundpool, mediaplayer, etc)?
// If false, native PCM mixing will always be used.
#define PREFER_PLATFORM_AUDIO_MANAGEMENT TRUE
// Number of simultaneous audio streams for mixing sound samples
#define MAX_AUDIO_STREAMS 20
// Maximum number of simultaneous PCM sounds loadable when using native PCM mixing
#define MAX_PCM_SOUNDS 200
// Playback rate - must match target platforms audio bridge/player
#define PCM_AUDIO_PLAYBACK_RATE 44100
// Playback channels - must match target platforms audio bridge/player
#define PCM_AUDIO_PLAYBACK_CHANNELS 2
// Number of buffers used when playing streaming sound.
// Increase this and decrease buffer size to lower decoding lag.
#define PCM_AUDIO_STREAM_BUFFERS 2
// The size of each streaming buffer.  Decrease to lower decoding lag (increases risk of underrun).
// Must be bigger than VORBIS_MAX_LEGAL_FRAME
#define PCM_AUDIO_STREAM_BUFFER_SIZE 64000
// How much data (in bytes) is read from disk/package for each chunked read.
#define CHUNKED_READ_BUFFER_SIZE 16384
// Do not change unless you know what you're doing.
#define VORBIS_MAX_LEGAL_FRAME 8192

#endif /* BATTERYTECH_GLOBALS_H_ */
