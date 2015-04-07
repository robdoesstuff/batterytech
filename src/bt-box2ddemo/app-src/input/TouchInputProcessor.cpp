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
#include <batterytech/audio/AudioManager.h>

TouchInputProcessor::TouchInputProcessor() {
	lastTouchDown = new BOOL32[10];
	for (S32 i = 0; i < 10; i++) {
		lastTouchDown[i] = FALSE;
	}
	touchedObj = new GameObject*[10];
	for (S32 i = 0; i < 10; i++) {
		touchedObj[i] = NULL;
	}
}

TouchInputProcessor::~TouchInputProcessor() {
	delete [] lastTouchDown;
	delete [] touchedObj;
}

void TouchInputProcessor::processInput(GameContext *context) {
	if (context->world->gameState != GAMESTATE_RUNNING) {
		return;
	}
	World *world = context->world;
	// handle object touch
	S32 i;
	for (i = 0; i < 10; i++) {
		BOOL32 down = context->pointerState[i].isDown;
		F32 worldX = GameUtil::screenToWorldX(context->pointerState[i].x, context);
		F32 worldY = GameUtil::screenToWorldY(context->pointerState[i].y, context);
		if (down && !lastTouchDown[i]) {
			// is down and was up = TOUCH DOWN
			context->audioManager->playSound("click.ogg", 0, 1.0f, 1.0f, 1.0f);
			GameObject *obj = GameUtil::findObjectIntersection(worldX, worldY, world);
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
}
