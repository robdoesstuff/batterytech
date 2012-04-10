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
	game = new Game(this);
	world = new World;
	worldRenderer = new WorldRenderer(this);
}

GameContext::~GameContext() {
	delete worldRenderer;
	worldRenderer = NULL;
	delete world;
	world = NULL;
	delete game;
	game = NULL;
}
