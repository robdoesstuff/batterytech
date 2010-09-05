//============================================================================
// Name        : batterytech.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include "batterytech.h"
#include "logger.h"
#include "sound/SoundManager.h"
#include "platform/platformgeneral.h"
#include "primitives.h"
#include "game/World.h"
#include "render/WorldRenderer.h"

static SoundManager *soundManager;
static World *world;
static WorldRenderer *worldRenderer;

void loadSound();

void btInit(S32 width, S32 height) {
	log("BatteryTech 1.0 Initializing...");
	world = new World;
	worldRenderer = new WorldRenderer;
	worldRenderer->init(width, height);
	log("Ready");
	loadSound();
}

void btSetScreenSize(int width, int height) {
}

void loadSound() {
	log("Loading sound");
	soundManager = new SoundManager;
	soundManager->init(10);
	U16 sndId = soundManager->loadSound("level_1_song.ogg");
	//U16 sndId2 = soundManager->loadSound("battery_powered_splash.ogg");
	//U16 sndId3 = soundManager->loadSound("score_session_end_big.ogg");
	soundManager->playSound(sndId, -1, 1.0f);
	//soundManager->playSound(sndId2, -1, 1.0f);
	//soundManager->playSound(sndId3, -1, 1.0f);
}


void btUpdate(F32 delta) {
	world->tickDelta = delta;
	//char buf[32];
	//sprintf(buf, "update %3.5g theta is %3.5g", delta, theta);
	//log(buf);
}


void btDraw() {
	worldRenderer->render(world);
}

void btSuspend() {
}

void btResume() {
}

void btSetPointerState(S32 pointerId, BOOL32 down, S32 x, S32 y) {
	//char buf[32];
	if (down) {
		if (pointerId == 0) {
			world->down1 = TRUE;
			world->x1 = x;
			world->y1 = y;
		} else if (pointerId == 1) {
			world->down2 = TRUE;
			world->x2 = x;
			world->y2 = y;
		}
		//sprintf(buf, "pointer %d down at %d %d", pointerId, x, y);
	} else {
		if (pointerId == 0) {
			world->down1 = FALSE;
		} else if (pointerId == 1) {
			world->down2 = FALSE;
		}
		//sprintf(buf, "pointer %d up", pointerId);
	}
	//log(buf);
}

void btRelease() {
	if (soundManager) {
		soundManager->release();
		delete soundManager;
	}
	soundManager = NULL;
}
