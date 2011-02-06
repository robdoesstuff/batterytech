/*
 * TouchInputProcessor.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rgreen
 */

#include "TouchInputProcessor.h"

#include "../World.h"
#include "../gameobject/GameObject.h"
#include "../GameUtil.h"
#include <batterytech/render/GraphicsConfiguration.h>
#include "../GameConstants.h"

TouchInputProcessor::TouchInputProcessor() {
	lastTouchDown = new BOOL32[2];
	lastTouchDown[0] = FALSE;
	lastTouchDown[1] = FALSE;
	touchedObj = new GameObject*[2];
	touchedObj[0] = NULL;
	touchedObj[1] = NULL;
}

TouchInputProcessor::~TouchInputProcessor() {
	delete [] lastTouchDown;
	delete [] touchedObj;
}

void TouchInputProcessor::processInput(Context *context) {
	if (context->world->gameState != GAMESTATE_RUNNING) {
		return;
	}
	World *world = context->world;
	// handle object touch
	world->down1 = context->down1;
	world->down2 = context->down2;
	world->x1 = context->x1;
	world->x2 = context->x2;
	world->y1 = context->y1;
	world->y2 = context->y2;
	S32 i;
	BOOL32 touchOnLauncher[2];
	for (i = 0; i < 2; i++) {
		touchOnLauncher[i] = FALSE;
		BOOL32 down;
		F32 worldX, worldY;
		if (i == 0) {
			down = context->down1;
			worldX = GameUtil::screenToWorldX(context->x1, context);
			worldY = GameUtil::screenToWorldY(context->y1, context);
		} else if (i == 1) {
			down = context->down2;
			worldX = GameUtil::screenToWorldX(context->x2, context);
			worldY = GameUtil::screenToWorldY(context->y2, context);
		}
		if (down && !lastTouchDown[i]) {
			GameObject *obj = GameUtil::findObjectIntersection(worldX, worldY, world);
			// is down and was up = TOUCH DOWN
			if (obj) {
				touchedObj[i] = obj;
				obj->onTouchDown(worldX, worldY);
			}
		} else if (down && lastTouchDown[i]) {
			// is down and was down = TOUCH MOVE
			if (touchedObj[i]) {
				touchedObj[i]->onTouchMove(worldX, worldY);
			}
		} else if (!down && lastTouchDown[i]) {
			// is up and was down = TOUCH UP
			if (touchedObj[i]) {
				touchedObj[i]->onTouchUp(worldX, worldY);
			}
			touchedObj[i] = NULL;
		}
		lastTouchDown[i] = down;
	}
	world->leftFlipperPressed = FALSE;
	world->rightFlipperPressed = FALSE;
	if (context->down1) {
		if (!touchOnLauncher[0]) {
			if (context->x1 < context->gConfig->viewportWidth / 2) {
				world->leftFlipperPressed = TRUE;
			} else {
				world->rightFlipperPressed = TRUE;
			}
			if (context->y1 > context->gConfig->viewportHeight - context->gConfig->viewportHeight / 10) {
				world->leftFlipperPressed = TRUE;
				world->rightFlipperPressed = TRUE;
			}
		}
	}
	if (context->down2) {
		if (!touchOnLauncher[1]) {
			if (context->x2 < context->gConfig->viewportWidth / 2) {
				world->leftFlipperPressed = TRUE;
			} else {
				world->rightFlipperPressed = TRUE;
			}
			if (context->y2 > context->gConfig->viewportHeight - context->gConfig->viewportHeight / 10) {
				world->leftFlipperPressed = TRUE;
				world->rightFlipperPressed = TRUE;
			}
		}
	}
}
