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
#include "game/World.h"
#include "render/WorldRenderer.h"
#include "ui/Button.h"
#include "ui/LinearLayout.h"
#include "render/MenuRenderer.h"
#include "Context.h"
#include "menus/MainMenu.h"
#include "menus/OptionsMenu.h"

#define REFERENCE_WIDTH 800
#define REFERENCE_HEIGHT 480

static Context *context;
static GraphicsConfiguration *gConfig;

void loadSound();
void createMenu(S32 width, S32 height);

void btInit(GraphicsConfiguration *graphicsConfig, S32 width, S32 height) {
	logmsg("BatteryTech 1.0 Initializing...");
	gConfig = graphicsConfig;
	// platform will have determined gpu capabilities and set into gConfig
	btSetScreenSize(width, height);
	context = new Context(gConfig);
	loadSound();
	createMenu(width, height);
	context->menuRenderer->init(context->uiManager);
	logmsg("Ready");
}

void btSetScreenSize(S32 width, S32 height) {
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

void loadSound() {
	logmsg("Loading sound");
	U16 sndId = context->soundManager->loadSound("level_1_song.ogg");
	//U16 sndId2 = soundManager->loadSound("battery_powered_splash.ogg");
	//U16 sndId3 = soundManager->loadSound("score_session_end_big.ogg");
	//soundManager->playSound(sndId, -1, 1.0f);
	//soundManager->playSound(sndId2, -1, 1.0f);
	//soundManager->playSound(sndId3, -1, 1.0f);
}

void createMenu(S32 width, S32 height) {
	Menu *mainMenu = new MainMenu(context);
	Menu *optionsMenu = new OptionsMenu(context);
	context->uiManager->addMenu(mainMenu);
	context->uiManager->addMenu(optionsMenu);
	context->uiManager->showMenu(MAIN_MENU_NAME);
}

void btUpdate(F32 delta) {
	// todo - just tick the game engine with the delta
	context->tickDelta = delta;
	context->uiManager->update();
	context->game->update();
	//char buf[32];
	//sprintf(buf, "update %3.5g theta is %3.5g", delta, theta);
	//log(buf);
}


void btDraw() {
	context->worldRenderer->render(context->world);
	context->menuRenderer->render(context->uiManager);
}

void btSuspend() {
}

void btResume() {
}

void btSetPointerState(S32 pointerId, BOOL32 down, S32 x, S32 y) {
	// TODO - move this all to app context - entire app needs it
	// TODO - use simple isFocused to let a menu or the game know it is foreground in layers
	// TODO - everything should just use poll model with isFocused to determine clicks, etc
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

void btRelease() {
	gConfig = NULL;
	delete context;
	context = NULL;
}
