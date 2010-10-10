/*
 * Game.cpp
 *
 *  Created on: Oct 10, 2010
 *      Author: rgreen
 */

#include "Game.h"

Game::Game(Context *context) {
	this->context = context;
}

Game::~Game() {
	context = NULL;
}

void Game::update() {
	context->world->theta += context->tickDelta * 60;
	context->world->down1 = context->down1;
	context->world->down2 = context->down2;
	context->world->x1 = context->x1;
	context->world->x2 = context->x2;
	context->world->y1 = context->y1;
	context->world->y2 = context->y2;
}

