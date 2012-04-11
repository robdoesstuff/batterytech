/*
 * BallRenderer.cpp
 *
 *  Created on: Dec 5, 2010
 *      Author: rgreen
 */

#include "BallRenderer.h"
#include "../gameobjects/Ball.h"
#include <batterytech/render/RenderContext.h>

BallRenderer::BallRenderer(GameContext *context) {
	this->context = context;
	this->spriteRenderer = new BatchSpriteRenderer(context, NULL);
}

BallRenderer::~BallRenderer() {
}

void BallRenderer::init(BOOL32 newGameContext) {
	spriteRenderer->setSpriteAssetname("smiley_tex.png");
	spriteRenderer->init(newGameContext);
}

void BallRenderer::render(World *world) {
	context->renderContext->colorFilter = Vector4f(1,1,1,1);
	S32 i;
	spriteRenderer->startBatch();
	for (i = 0; i < world->gameObjects->getSize(); i++) {
		Ball *ball = (Ball*) world->gameObjects->array[i];
		spriteRenderer->render(ball->x, ball->y, BALL_RADIUS * 2, BALL_RADIUS * 2, ball->angle);
	}
	spriteRenderer->endBatch();
}

