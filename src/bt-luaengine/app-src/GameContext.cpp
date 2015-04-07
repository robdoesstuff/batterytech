/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
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
