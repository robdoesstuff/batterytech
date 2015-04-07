/*
 * GameContext.cpp
 *
 *  Created on: Apr 9, 2012
 *      Author: rgreen
 */

#include "GameContext.h"
#include "Game.h"
#include "World.h"
#include "render/WorldRenderer.h"

GameContext::GameContext(GraphicsConfiguration *gConfig) : Context(gConfig) {
	if (!appProperties) {
		world = NULL;
		game = NULL;
		worldRenderer = NULL;
		return;
	}
	world = new World;
	game = new Game(this);
	worldRenderer = new WorldRenderer(this);
	appUpdater = game;
	appRenderer = worldRenderer;
}

GameContext::~GameContext() {
	delete worldRenderer;
	worldRenderer = NULL;
	delete world;
	world = NULL;
	delete game;
	game = NULL;
}
