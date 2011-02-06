/*
 * PinballRenderer.cpp
 *
 *  Created on: Dec 5, 2010
 *      Author: rgreen
 */

#include "PinballRenderer.h"
#include "../gameobjects/Pinball.h"

PinballRenderer::PinballRenderer(Context *context) {
	this->context = context;
	this->spriteRenderer = new SimpleSpriteRenderer(context, NULL);
}

PinballRenderer::~PinballRenderer() {
}

void PinballRenderer::init(BOOL32 newContext) {
	spriteRenderer->setSpriteAssetname("smiley_tex.png");
	spriteRenderer->init(newContext);
}

void PinballRenderer::render(World *world) {
	glColor4f(1,1,1,1);
	S32 i;
	for (i = 0; i < world->pinballs->getSize(); i++) {
		Pinball *pinball = (Pinball*) world->pinballs->array[i];
		spriteRenderer->render(pinball->x, pinball->y, PINBALL_RADIUS * 2, PINBALL_RADIUS * 2, pinball->angle);
	}
}

void PinballRenderer::renderBallsRemaining(World *world) {
	S32 i;
	// render pinballs remaining
	for (i = 0; i < world->ballsRemaining; i++) {
		spriteRenderer->render(PINBALL_RADIUS * 1.25f + PINBALL_RADIUS * 2.5f * i, WORLD_TOP - PINBALL_RADIUS * 1.25f, PINBALL_RADIUS * 2, PINBALL_RADIUS * 2, 0);
	}
}
