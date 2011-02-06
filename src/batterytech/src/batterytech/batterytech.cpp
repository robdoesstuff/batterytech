//============================================================================
// Name        : batterytech.cpp
// Author      : Robert Green
// Version     : 1.0
// Copyright   : Copyright 2010 Battery Powered Games, LLC
// Description : main hooks for battery tech mobile game platform
//============================================================================

#include <stdio.h>
#include "batterytech.h"
#include "Logger.h"
#include "sound/SoundManager.h"
#include "platform/platformgl.h"
#include "platform/platformgeneral.h"
#include "ui/Button.h"
#include "ui/LinearLayout.h"
#include "render/MenuRenderer.h"
#include "Context.h"
#include "menus/MainMenu.h"
#include "menus/OptionsMenu.h"
#include "../demo-app/Game.h"
#include "../demo-app/render/WorldRenderer.h"
#include <stdlib.h>
#include <time.h>

#define REFERENCE_WIDTH 480
#define REFERENCE_HEIGHT 800

static Context *context;
static GraphicsConfiguration *gConfig;

void createMenu(S32 width, S32 height);

void btInit(GraphicsConfiguration *graphicsConfig, S32 width, S32 height) {
	logmsg("BatteryTech 1.0 Initializing...");
	gConfig = graphicsConfig;
	// platform will have determined gpu capabilities and set into gConfig
	btSetScreenSize(width, height);
	if (context) {
		logmsg("Context already exists!!  Check for missing shutdown calls");
	}
	context = new Context(gConfig);
	createMenu(width, height);
	// initialize random number generator
	srand(time(NULL));
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

void createMenu(S32 width, S32 height) {
	Menu *mainMenu = new MainMenu(context);
	Menu *optionsMenu = new OptionsMenu(context);
	context->uiManager->addMenu(mainMenu);
	context->uiManager->addMenu(optionsMenu);
	//context->uiManager->showMenu(MAIN_MENU_NAME);
}

void btUpdate(F32 delta) {
	//logmsg("btUpdate");
	context->tickDelta = delta;
	context->uiManager->update();
	context->game->update();
	// clear key pressed.
	context->keyPressed = 0;
}


void btDraw() {
	//logmsg("btDraw");
	context->worldRenderer->render(context->world);
	context->menuRenderer->render(context->uiManager);
}

void btSuspend() {
	context->wasSuspended = TRUE;
}

void btResume() {
}

void btSetPointerState(S32 pointerId, BOOL32 down, S32 x, S32 y) {
	//logmsg("btSetPointerState");
	if (!context) {
		return;
	}
	//char buf[32];
	if (down) {
		if (pointerId == 0) {
			context->down1 = TRUE;
			context->x1 = x;
			context->y1 = y;
		} else if (pointerId == 1) {
			context->down2 = TRUE;
			context->x2 = x;
			context->y2 = y;
		}
		//sprintf(buf, "pointer %d down at %d %d", pointerId, x, y);
	} else {
		if (pointerId == 0) {
			context->down1 = FALSE;
		} else if (pointerId == 1) {
			context->down2 = FALSE;
		}
		//sprintf(buf, "pointer %d up", pointerId);
	}
	//log(buf);
}

void btKeyUp(U8 key) {
	char buf[20];
	sprintf(buf, "KeyUp=%c, (%d)", key, key);
	//logmsg(buf);
}

void btKeyDown(U8 key) {
	char buf[20];
	sprintf(buf, "KeyDown=%c, (%d)", key, key);
	//logmsg(buf);
}

void btKeyPressed(U8 key) {
	char buf[20];
	sprintf(buf, "KeyPressed=%c, (%d)", key, key);
	logmsg(buf);
	context->keyPressed = key;
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
