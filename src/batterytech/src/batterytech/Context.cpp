/*
 * Context.cpp
 *
 *  Created on: Oct 8, 2010
 *      Author: rgreen
 */

#include "Context.h"

Context::Context(GraphicsConfiguration *gConfig) {
	this->gConfig = gConfig;
	world = new World;
	// TODO - read in preferences and load into GraphicsConfiguration
	worldRenderer = new WorldRenderer(gConfig);
	worldRenderer->init();
	menuRenderer = new MenuRenderer(gConfig);
	soundManager = new SoundManager;
	soundManager->init(10);
	game = new Game(this);
	uiManager = new UIManager(this);
}

Context::~Context() {
	if (soundManager) {
		soundManager->release();
		delete soundManager;
	}
	soundManager = NULL;
	delete worldRenderer;
	delete world;
	// TODO - release uimanager
	delete uiManager;

}
