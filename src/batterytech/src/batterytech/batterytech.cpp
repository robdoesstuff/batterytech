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

#define REFERENCE_WIDTH 800
#define REFERENCE_HEIGHT 480

static Context *context;
static GraphicsConfiguration *gConfig;

void loadSound();
void createMenu(S32 width, S32 height);

void btInit(GraphicsConfiguration *graphicsConfig, S32 width, S32 height) {
	log("BatteryTech 1.0 Initializing...");
	gConfig = graphicsConfig;
	// platform will have determined gpu capabilities and set into gConfig
	btSetScreenSize(width, height);
	context = new Context(gConfig);
	loadSound();
	createMenu(width, height);
	context->menuRenderer->init(context->uiManager);
	log("Ready");
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
	log("Loading sound");
	U16 sndId = context->soundManager->loadSound("level_1_song.ogg");
	//U16 sndId2 = soundManager->loadSound("battery_powered_splash.ogg");
	//U16 sndId3 = soundManager->loadSound("score_session_end_big.ogg");
	//soundManager->playSound(sndId, -1, 1.0f);
	//soundManager->playSound(sndId2, -1, 1.0f);
	//soundManager->playSound(sndId3, -1, 1.0f);
}

void createMenu(S32 width, S32 height) {
	S32 buttonBgId = context->menuRenderer->addTextureAsset("button1_tex.png");
	S32 panelBgId = context->menuRenderer->addTextureAsset("panel1_tex.png");
	LinearLayout *buttonLayout = new LinearLayout(LinearLayout::VERTICAL);
	Button *button1 = new Button();
	button1->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	button1->setSize(160, 60);
	button1->setMargins(5);
	button1->setBackgroundMenuResource(buttonBgId);
	button1->setText("Button 1");
	Button *button2 = new Button();
	button2->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	button2->setSize(160, 60);
	button2->setMargins(5);
	button2->setBackgroundMenuResource(buttonBgId);
	button2->setText("Button 2");
	Button *button3 = new Button();
	button3->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	button3->setSize(160, 60);
	button3->setMargins(5);
	button3->setBackgroundMenuResource(buttonBgId);
	button3->setText("Button 3");
	buttonLayout->addComponent(button1);
	buttonLayout->addComponent(button2);
	buttonLayout->addComponent(button3);
	buttonLayout->setHeight(FILL);
	buttonLayout->setWidth(WRAP);
	buttonLayout->setBackgroundMenuResource(panelBgId);
	buttonLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::TOP));
	LinearLayout *bottomLayout = new LinearLayout(LinearLayout::HORIZONTAL);
	Button *button4 = new Button();
	button4->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	button4->setSize(90, 60);
	button4->setMargins(5);
	button4->setBackgroundMenuResource(buttonBgId);
	button4->setText("A");
	Button *button5 = new Button();
	button5->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	button5->setSize(90, 60);
	button5->setMargins(5);
	button5->setBackgroundMenuResource(buttonBgId);
	button5->setText("B");
	bottomLayout->addComponent(button4);
	bottomLayout->addComponent(button5);
	bottomLayout->setWidth(WRAP);
	bottomLayout->setHeight(WRAP);
	bottomLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::BOTTOM));
	buttonLayout->addComponent(bottomLayout);
	Menu *mainMenu = new Menu(buttonLayout);
	S32 mainMenuId = context->uiManager->addMenu(mainMenu);
	context->uiManager->showMenu(mainMenuId);
}

void btUpdate(F32 delta) {
	// todo - just tick the game engine with the delta
	context->world->tickDelta = delta;
	context->uiManager->updateMenus(delta);
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
	World *world = context->world;
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
	gConfig = NULL;
	delete context;
	context = NULL;
}
