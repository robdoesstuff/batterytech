/*
 * BallRenderer.cpp
 *
 *  Created on: Dec 5, 2010
 *      Author: rgreen
 */

#include "BallRenderer.h"
#include "../gameobjects/Ball.h"

BallRenderer::BallRenderer(Context *context) {
	this->context = context;
	this->spriteRenderer = new SimpleSpriteRenderer(context, NULL);
}

BallRenderer::~BallRenderer() {
}

void BallRenderer::init(BOOL32 newContext) {
	spriteRenderer->setSpriteAssetname("smiley_tex.png");
	spriteRenderer->init(newContext);
}

void BallRenderer::render(World *world) {
	glColor4f(1,1,1,1);
	S32 i;
	for (i = 0; i < world->gameObjects->getSize(); i++) {
		Ball *ball = (Ball*) world->gameObjects->array[i];
		spriteRenderer->render(ball->x, ball->y, BALL_RADIUS * 2, BALL_RADIUS * 2, ball->angle);
	}
}

