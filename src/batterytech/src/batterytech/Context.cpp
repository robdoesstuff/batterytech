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
#include "audio/AudioManager.h"
#include "network/NetworkManager.h"
#include "VibrationManager.h"
#include "render/GraphicsConfiguration.h"
#include "ui/UIManager.h"
#include "render/RenderContext.h"

#define MAX_AUDIO_STREAMS 20

Context::Context(GraphicsConfiguration *gConfig) {
	this->gConfig = gConfig;
	this->world = new World;
	worldRenderer = new WorldRenderer(this);
	menuRenderer = new MenuRenderer(this);
	audioManager = new AudioManager;
	audioManager->init(MAX_AUDIO_STREAMS);
	networkManager = new NetworkManager(this);
	vibrationManager = new VibrationManager(this);
	game = new Game(this);
	uiManager = new UIManager(this);
	renderContext = new RenderContext();
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
	if (audioManager) {
		delete audioManager;
	}
	audioManager = NULL;
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
	delete renderContext;
	renderContext = NULL;
	logmsg("Context deleting game");
	delete game;
	logmsg("Context nulling game");
	game = NULL;
	delete [] pointerState;
	logmsg("Done Releasing Context");
}
