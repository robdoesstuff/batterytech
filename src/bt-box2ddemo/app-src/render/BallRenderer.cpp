
/*
 * BallRenderer.cpp
 *
 *  Created on: Dec 5, 2010
 *      Author: rgreen
 */

#include "BallRenderer.h"
#include "../gameobjects/Ball.h"
#include <batterytech/render/RenderContext.h>
#include <batterytech/render/QuadRenderer.h>
#include <batterytech/render/GLResourceManager.h>
#include <batterytech/render/AssetTexture.h>

BallRenderer::BallRenderer(GameContext *context) {
	this->context = context;
}

BallRenderer::~BallRenderer() {
}

void BallRenderer::init(BOOL32 newGameContext) {
    if (!context->glResourceManager->getTexture("smiley_tex.png")) {
        AssetTexture *t = new AssetTexture(context, "smiley_tex.png");
        t->load();
        context->glResourceManager->addTexture(t);
    }
}

void BallRenderer::render(World *world) {
	context->renderContext->colorFilter = Vector4f(1,1,1,1);
	S32 i;
    context->quadRenderer->startBatch();
    Texture *t = context->glResourceManager->getTexture("smiley_tex.png");
	for (i = 0; i < world->gameObjects->getSize(); i++) {
		Ball *ball = (Ball*) world->gameObjects->array[i];
		context->quadRenderer->render(t, ball->x, ball->y, BALL_RADIUS * 2, BALL_RADIUS * 2, ball->angle);
	}
	context->quadRenderer->endBatch();
}

