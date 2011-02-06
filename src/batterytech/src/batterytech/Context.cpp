/*
 * Context.cpp
 *
 *  Created on: Oct 8, 2010
 *      Author: rgreen
 */

#include "Context.h"
#include "../demo-app/Game.h"
#include "../demo-app/World.h"
#include "../demo-app/render/WorldRenderer.h"
#include "render/MenuRenderer.h"
#include "sound/SoundManager.h"
#include "network/NetworkManager.h"
#include "VibrationManager.h"
#include "render/GraphicsConfiguration.h"
#include "ui/UIManager.h"

Context::Context(GraphicsConfiguration *gConfig) {
	this->gConfig = gConfig;
	this->world = new World;
	worldRenderer = new WorldRenderer(this);
	menuRenderer = new MenuRenderer(gConfig);
	soundManager = new SoundManager;
	soundManager->init(10);
	networkManager = new NetworkManager(this);
	vibrationManager = new VibrationManager(this);
	game = new Game(this);
	uiManager = new UIManager(this);
	tickDelta = 0;
	isUIConsumingTouch = FALSE;
	keyPressed = 0;
	showFPS = FALSE;
	wasSuspended = TRUE;
	accelerometerState.x = 0;
	accelerometerState.y = 0;
	accelerometerState.z = 0;
	pointerState = new PointerState[10];
	for (S32 i = 0; i < 10; i++) {
		pointerState[i].isDown = FALSE;
		pointerState[i].x = 0;
		pointerState[i].y = 0;
	}
}

Context::~Context() {
	logmsg("Releasing Context");
	if (soundManager) {
		delete soundManager;
	}
	soundManager = NULL;
	delete networkManager;
	networkManager = NULL;
	delete vibrationManager;
	vibrationManager = NULL;
	delete worldRenderer;
	worldRenderer = NULL;
	delete world;
	world = NULL;
	delete uiManager;
	uiManager = NULL;
	logmsg("Context deleting game");
	delete game;
	logmsg("Context nulling game");
	game = NULL;
	delete [] pointerState;
	logmsg("Done Releasing Context");
}
