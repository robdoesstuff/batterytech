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
#include "../demo-app/Game.h"
#include "../demo-app/render/WorldRenderer.h"
#include <stdlib.h>
#include <time.h>
#include "batterytech_globals.h"

static Context *context = NULL;
static GraphicsConfiguration *gConfig = NULL;

static F32 updateTimes[TICK_SMOOTHER_SAMPLES];
static S32 updateTimeIdx = 0;

void btInit(GraphicsConfiguration *graphicsConfig, S32 width, S32 height) {
	char buf[255];
	sprintf(buf, "BatteryTech %s Initializing...", BATTERYTECH_VERSION);
	logmsg(buf);
	gConfig = graphicsConfig;
	// platform will have determined gpu capabilities and set into gConfig
	btSetScreenSize(width, height);
	if (context) {
		logmsg("Context already exists!!  Check for missing shutdown calls");
	}
	context = new Context(gConfig);
	// initialize random number generator
	srand(time(NULL));
	for (S32 i = 0; i < TICK_SMOOTHER_SAMPLES; i++) {
		updateTimes[i] = -1;
	}
	if (graphicsConfig->supportsShaders && USE_SHADERS_WHEN_SUPPORTED) {
		graphicsConfig->useShaders = TRUE;
		logmsg("Using Shaders");
	} else {
		logmsg("Using Fixed-Function Rendering");
	}
	logmsg("Ready");
}

void btSetScreenSize(S32 width, S32 height) {
	//logmsg("btSetScreenSize");
	gConfig->width = width;
	gConfig->height = height;
	gConfig->viewportWidth = width;
	gConfig->viewportHeight = height;
	gConfig->scaleX2d = width / (F32)REFERENCE_WIDTH;
	gConfig->scaleY2d = height / (F32)REFERENCE_HEIGHT;
	if (width > height) {
		gConfig->uiScale = gConfig->scaleY2d;
	} else {
		gConfig->uiScale = gConfig->scaleX2d;
	}
}

void btUpdate(F32 delta) {
	if (!context->wasSuspended) {
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
		context->tickDelta = 0;
	}
	//char buf[50];
	//sprintf(buf, "update %f", delta);
	//logmsg(buf);
	//logmsg("btUpdate");
	//context->tickDelta = delta;
	context->audioManager->update();
	context->uiManager->update();
	context->game->update();
	// clear key pressed.
	context->keyPressed = 0;
	context->specialKeyPressed = BatteryTech::SKEY_NULL;
}


void btDraw() {
	//logmsg("btDraw");
	context->worldRenderer->render(context->world);
	context->menuRenderer->render();
}

void btSuspend() {
	context->wasSuspended = TRUE;
}

void btResume() {
}

void btSetPointerState(S32 pointerId, BOOL32 down, S32 x, S32 y) {
	//logmsg("btSetPointerState");
	/*
	// multitouch debugging
	char buf[100];
	if (down) {
		sprintf(buf, "Pointer %d down at %d %d", pointerId, x, y);
	} else {
		sprintf(buf, "Pointer %d up at %d %d", pointerId, x, y);
	}
	logmsg(buf);
	*/
	if (!context) {
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
}

void btKeyDown(U8 key, BatteryTech::SpecialKey sKey) {
	//char buf[20];
	//sprintf(buf, "KeyDown=%c, (%d)", key, key);
	//logmsg(buf);
}

void btKeyPressed(U8 key, BatteryTech::SpecialKey sKey) {
	//char buf[20];
	//sprintf(buf, "KeyPressed=%c, (%d)", key, key);
	//logmsg(buf);
	context->keyPressed = key;
	context->specialKeyPressed = sKey;
}

void btAccelerometerChanged(F32 x, F32 y, F32 z) {
	context->accelerometerState.x = x;
	context->accelerometerState.y = y;
	context->accelerometerState.z = z;
}

void btRelease() {
	logmsg("Batterytech Release Starting");
	_platform_stop_all_vibration_effects();
	gConfig = NULL;
	delete context;
	context = NULL;
	logmsg("Batterytech Release Complete");
}
