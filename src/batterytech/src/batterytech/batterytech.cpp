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
#include "platform/platformgl.h"
#include "platform/platformgeneral.h"
#include "game/World.h"
#include "render/WorldRenderer.h"
#include "ui/Button.h"
#include "ui/LinearLayout.h"
#include "ui/LinearLayoutParameters.h"

static SoundManager *soundManager;
static World *world;
static WorldRenderer *worldRenderer;
static GraphicsConfiguration *gConfig;

void loadSound();
void createMenu(S32 width, S32 height);

void btInit(GraphicsConfiguration *graphicsConfig, S32 width, S32 height) {
	log("BatteryTech 1.0 Initializing...");
	world = new World;
	// platform will have determined gpu capabilities and set into gConfig
	gConfig = graphicsConfig;
	// TODO - read in preferences and load into GraphicsConfiguration
	worldRenderer = new WorldRenderer(gConfig);
	worldRenderer->init(width, height);
	log("Ready");
	loadSound();
	createMenu(width, height);
}

void btSetScreenSize(S32 width, S32 height) {
	if (worldRenderer) {
		worldRenderer->setScreenSize(width, height);
	}
}

void loadSound() {
	log("Loading sound");
	soundManager = new SoundManager;
	soundManager->init(10);
	U16 sndId = soundManager->loadSound("level_1_song.ogg");
	//U16 sndId2 = soundManager->loadSound("battery_powered_splash.ogg");
	//U16 sndId3 = soundManager->loadSound("score_session_end_big.ogg");
	//soundManager->playSound(sndId, -1, 1.0f);
	//soundManager->playSound(sndId2, -1, 1.0f);
	//soundManager->playSound(sndId3, -1, 1.0f);
}

void createMenu(S32 width, S32 height) {
	LinearLayout *buttonLayout = new LinearLayout(LinearLayout::VERTICAL);
	Button *button1 = new Button();
	button1->setLayoutParameters(new LinearLayoutParameters(LinearLayoutParameters::LEFT, LinearLayoutParameters::TOP));
	button1->setSize(120, 40);
	Button *button2 = new Button();
	button2->setLayoutParameters(new LinearLayoutParameters(LinearLayoutParameters::LEFT, LinearLayoutParameters::TOP));
	button2->setSize(120, 40);
	buttonLayout->addComponent(button1);
	buttonLayout->addComponent(button2);
	// this part should happen in the setUI() method
	buttonLayout->setDrawableBounds(0, 0, width, height);
	//buttonLayout->layout(gConfig->uiScale);
	log("starting layout");
	buttonLayout->layout(1.0f);
	// TODO - dealloc this stuff
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
	delete worldRenderer;
	delete world;
}
