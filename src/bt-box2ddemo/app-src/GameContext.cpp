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
}

GameContext::~GameContext() {
}
