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
// Name        : batterytech_globals.h
// Description : Global Settings - Change as needed!
//============================================================================

#ifndef BATTERYTECH_GLOBALS_H_
#define BATTERYTECH_GLOBALS_H_

#include "primitives.h"

/// ---------------- General -----------------
// Configurable parts - needs to be in assets dir.  prepend "auto:" if you wnat to be able to override this from the external storage assets dir.
#define BT_CONFIGFILE "batterytech_config.txt"
// Log to file?  Use NULL for console-only logging
#define BT_LOGFILE "batterytech.log"
// Are we in debug mode? (this is currently unused in batterytech)
#define BT_DEBUG FALSE
// Number of samples to average for use as the tick delta in updates
// Many Android devices have erratic updates without smoothing.
#define TICK_SMOOTHER_SAMPLES 15
// Maximum number of touch/pointers we support
#define MAX_POINTERS 10
// Maximum number of simultaneous keys pressed we support
#define MAX_KEYSTATES 10

/// -------------- UI and Text --------------
// Maximum number of components you can add to any other UI component (including layouts).
// Increase if necessary.
#define MAX_UI_SUBCOMPONENTS 25
// Maximum number of UI rendering assets.
// Increase if necessary.
#define MAX_UI_ASSET_NAMES 300
// Maximum number of characters in a multiline of text
#define TEXT_RENDER_BUFFER_SIZE 2048

/// -------------- audio --------------------
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
// Maximum number of PCM Plugins
#define MAX_PCM_PLUGINS 10

/// ------------- models -------------------
// if we want to use OpenGL ES 2.0 shader GPU-accelerated animation
#define ASSIMP_GPU_ACCELERATED_RENDER TRUE

// misc
#define FORCE_SQUARE_TEXTURES FALSE
#define FORCE_32_BIT_TEXTURES FALSE
#define FORCE_16_BIT_RGB_TEXTURES FALSE

/// ------------- version ---------------
#define BATTERYTECH_VERSION "2.0.3"

#endif /* BATTERYTECH_GLOBALS_H_ */
