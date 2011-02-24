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

#define BT_DEBUG FALSE
#define USE_SHADERS_WHEN_SUPPORTED TRUE
#define REFERENCE_WIDTH 480
#define REFERENCE_HEIGHT 800
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 800
#define TICK_SMOOTHER_SAMPLES 15
#define WINDOWED_APP_NAME "BatteryTech Demo App"
#define CONSOLE_LOG_ENABLED_WHEN_AVAILABLE TRUE
// UI and Text
#define INITIAL_FONT_TEXTURE_WIDTH 256
#define INITIAL_FONT_TEXTURE_HEIGHT 256
#define MAX_UI_SUBCOMPONENTS 10
// audio
#define PREFER_PLATFORM_AUDIO_MANAGEMENT TRUE
#define MAX_AUDIO_STREAMS 20
#define MAX_PCM_SOUNDS 200
#define PCM_AUDIO_PLAYBACK_RATE 44100
#define PCM_AUDIO_PLAYBACK_CHANNELS 2
#define PCM_AUDIO_STREAM_BUFFERS 2
#define PCM_AUDIO_STREAM_BUFFER_SIZE 64000
#define CHUNKED_READ_BUFFER_SIZE 16384
#define VORBIS_MAX_LEGAL_FRAME 8192


#endif /* BATTERYTECH_GLOBALS_H_ */
