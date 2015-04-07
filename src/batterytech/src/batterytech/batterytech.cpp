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
// Name        : batterytech.cpp
// Description : main hooks for BatteryTech platform
//============================================================================

#include <stdio.h>
#include "batterytech.h"
#include "Logger.h"
#include "audio/AudioManager.h"
#include "platform/platformgl.h"
#include "platform/platformgeneral.h"
#include "ui/Button.h"
#include "ui/LinearLayout.h"
#include "render/MenuRenderer.h"
#include "Context.h"
#include <stdlib.h>
#include <time.h>
#include "batterytech_globals.h"
#include "util/strx.h"

static Context *context = NULL;
static GraphicsConfiguration *gConfig = NULL;

static F32 updateTimes[TICK_SMOOTHER_SAMPLES];
static S32 updateTimeIdx = 0;
static BOOL32 btReady = FALSE;
static BOOL32 btWasSuspended = FALSE;

// defined by BT application
extern Context* btAppCreateContext(GraphicsConfiguration *graphicsConfig);

void btInit(GraphicsConfiguration *graphicsConfig, S32 width, S32 height) {
	Logger::useFileOutput(BT_LOGFILE);
	btReady = FALSE;
	char buf[100];
	sprintf(buf, "BatteryTech %s Initializing...", BATTERYTECH_VERSION);
	logmsg(buf);
	gConfig = graphicsConfig;
	if (context) {
		logmsg("Context already exists!!  Check for missing shutdown calls");
	}
	context = btAppCreateContext(gConfig);
	if (context->appProperties) {
		btReady = TRUE;
	} else {
		logmsg("No Application Properties available.  Not Ready.");
		logmsg("Possible issues are:");
		logmsg("1) missing config file from assets");
		logmsg("2) running binary from a directory without assets available");
		return;
	}
	btSetScreenSize(width, height);
	// initialize random number generator
	srand(time(NULL));
	btClearTickDeltas();
	if (graphicsConfig->supportsShaders && context->appProperties->get("use_shaders")->getBoolValue()) {
		graphicsConfig->useShaders = TRUE;
		logmsg("Using Shaders");
	} else {
		logmsg("Using Fixed-Function Rendering");
	}
	// check/create save paths
	char path[1024];
	_platform_get_application_storage_dir_name(path, 512);
	if (!_platform_path_exists(path)) {
		char logbuf[1024];
		sprintf(logbuf, "Creating directory %s", path);
		logmsg(logbuf);
		_platform_path_create_recursive(path);
	}
	_platform_get_external_storage_dir_name(path, 512);
	if (!_platform_path_exists(path)) {
		char logbuf[1024];
		sprintf(logbuf, "Creating directory %s", path);
		logmsg(logbuf);
		_platform_path_create_recursive(path);
	}
	logmsg("Ready");
}

void btSetScreenSize(S32 width, S32 height) {
	if (!btReady) {
		return;
	}
	//logmsg("btSetScreenSize");
	gConfig->width = width;
	gConfig->height = height;
	gConfig->viewportWidth = width;
	gConfig->viewportHeight = height;
	gConfig->scaleX2d = width / context->appProperties->get("reference_width")->getFloatValue();
	gConfig->scaleY2d = height / context->appProperties->get("reference_height")->getFloatValue();
	if (width > height) {
		gConfig->uiScale = gConfig->scaleY2d;
	} else {
		gConfig->uiScale = gConfig->scaleX2d;
	}
}

void btClearTickDeltas() {
	for (S32 i = 0; i < TICK_SMOOTHER_SAMPLES; i++) {
		updateTimes[i] = -1;
	}
	updateTimeIdx = 0;
	context->tickDelta = 0;
}

void btUpdate(F32 delta) {
	if (!btReady) {
		return;
	}
	if (!btWasSuspended) {
		updateTimes[updateTimeIdx++] = delta;
		updateTimeIdx %= TICK_SMOOTHER_SAMPLES;
		if (updateTimes[updateTimeIdx] != -1) {
			// filled
			F32 total = 0;
			for (S32 i = 0; i < TICK_SMOOTHER_SAMPLES; i++) {
				total += updateTimes[i];
			}
			F32 avg = total / TICK_SMOOTHER_SAMPLES;
			//char buf[50];
			//sprintf(buf, "avg %f", avg);
			//logmsg(buf);
			context->tickDelta = avg;
		} else {
			context->tickDelta = delta;
		}
	} else {
		// reset the tick delta after a suspend
		btClearTickDeltas();
		btWasSuspended = FALSE;
	}
	//char buf[50];
	//sprintf(buf, "update %f", delta);
	//logmsg(buf);
	//logmsg("btUpdate");
	//context->tickDelta = delta;
	context->audioManager->update();
	context->uiManager->update();
	if (context->appUpdater) {
		context->appUpdater->update();
	}
	// clear key pressed.
	context->keyPressed = 0;
	context->specialKeyPressed = BatteryTech::SKEY_NULL;
}

void btDraw() {
	if (!btReady) {
		return;
	}
	//logmsg("btDraw");
	if (context->appRenderer) {
		context->appRenderer->render();
	}
}

void btSuspend() {
	if (context && context->appUpdater) {
		context->appUpdater->onSuspend();
	}
	btWasSuspended = TRUE;
}

void btSetGraphicsContextLost(BOOL32 wasLost) {
	if (context) {
		context->newGraphicsContext = wasLost;
	}
}

void btResume() {
	if (context && context->appUpdater) {
		context->appUpdater->onResume();
	}
}

void btSetPointerState(S32 pointerId, BOOL32 down, S32 x, S32 y) {
	//logmsg("btSetPointerState");
	
	// multitouch debugging
    /*
	char buf[100];
	if (down) {
		sprintf(buf, "Pointer %d down at %d %d", pointerId, x, y);
	} else {
		sprintf(buf, "Pointer %d up at %d %d", pointerId, x, y);
	}
	logmsg(buf);
     */
	
	if (!context || !btReady) {
		return;
	}
	if (pointerId < 10 && pointerId > -1) {
		context->pointerState[pointerId].isDown = down;
		context->pointerState[pointerId].x = x;
		context->pointerState[pointerId].y = y;
	}
}

void btKeyUp(U8 key, BatteryTech::SpecialKey sKey) {
	//char buf[20];
	//sprintf(buf, "KeyUp=%c, (%d)", key, key);
	//logmsg(buf);
	if (!context || !btReady) {
		return;
	}
	for (S32 i = 0; i < MAX_KEYSTATES; i++) {
		if (context->keyState[i].isDown && context->keyState[i].keyCode == key) {
			context->keyState[i].isDown = FALSE;
			context->keyState[i].keyCode = 0;
			break;
		}
	}
}

void btKeyDown(U8 key, BatteryTech::SpecialKey sKey) {
	//char buf[20];
	//sprintf(buf, "KeyDown=%c, (%d)", key, key);
	//logmsg(buf);
	if (!context || !btReady) {
		return;
	}
	BOOL32 alreadyDown = FALSE;
	for (S32 i = 0; i < MAX_KEYSTATES; i++) {
		if (context->keyState[i].isDown && context->keyState[i].keyCode == key) {
			alreadyDown = TRUE;
			break;
		}
	}
	if (!alreadyDown) {
		for (S32 i = 0; i < MAX_KEYSTATES; i++) {
			if (!context->keyState[i].isDown) {
				context->keyState[i].isDown = TRUE;
				context->keyState[i].keyCode = key;
				break;
			}
		}
	}
}

void btKeyPressed(U8 key, BatteryTech::SpecialKey sKey) {
	//char buf[20];
	//sprintf(buf, "KeyPressed=%c, (%d)", key, key);
	//logmsg(buf);
	if (context) {
		context->keyPressed = key;
		context->specialKeyPressed = sKey;
	}
}

void btAccelerometerChanged(F32 x, F32 y, F32 z) {
	if (context && btReady) {
		context->accelerometerState.x = x;
		context->accelerometerState.y = y;
		context->accelerometerState.z = z;
	}
}

BOOL32 btCallback(const char *data) {
	if (context) {
		if (context->callbackDataReady) {
			return FALSE;
		}
		strcpy(context->callbackData, data);
		context->callbackDataReady = TRUE;
		return TRUE;
	}
	return FALSE;
}

void btRelease() {
	logmsg("Batterytech Release Starting");
	_platform_stop_all_vibration_effects();
	gConfig = NULL;
	delete context;
	context = NULL;
	logmsg("Batterytech Release Complete");
}

Context* btGetContext() {
	return context;
}
