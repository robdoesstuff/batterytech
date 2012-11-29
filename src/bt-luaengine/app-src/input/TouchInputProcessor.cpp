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

#include "TouchInputProcessor.h"

#include "../World.h"
#include "../ScreenControl.h"
#include "../GameUtil.h"
#include <batterytech/render/GraphicsConfiguration.h>
#include "../GameConstants.h"
#include "../Game.h"

TouchInputProcessor::TouchInputProcessor() {
	lastTouchDown = new BOOL32[10];
	for (S32 i = 0; i < 10; i++) {
		lastTouchDown[i] = FALSE;
	}
	touchedControl = new ScreenControl*[10];
	for (S32 i = 0; i < 10; i++) {
		touchedControl[i] = NULL;
	}
}

TouchInputProcessor::~TouchInputProcessor() {
	delete [] lastTouchDown;
	delete [] touchedControl;
}

void TouchInputProcessor::processInput(GameContext *context) {
	if (context->world->gameState != GAMESTATE_RUNNING) {
		return;
	}
	World *world = context->world;
	ManagedArray<ScreenControl> *controls = world->screenControls;
	// handle object touch
	S32 i,j;
	for (i = 0; i < 10; i++) {
		BOOL32 down = context->pointerState[i].isDown;
		F32 px = context->pointerState[i].x;
		F32 py = context->pointerState[i].y;
		if (down && !lastTouchDown[i]) {
			// is down and was up = TOUCH DOWN
			ScreenControl *obj = NULL;
			for (j = 0; j < controls->getSize(); j++) {
				ScreenControl *sc = controls->array[j];
				if (sc->isInteractive) {
					Vector4f &bounds = sc->pointerBounds;
					if (px < bounds.x || px > bounds.z || py < bounds.y || py > bounds.w) {
						// early out
					} else {
						obj = sc;
						break;
					}
				}
			}
			if (obj) {
				touchedControl[i] = obj;
				context->game->luaBinder->screenControlPointerDown(obj->getName(), px, py);
			}
		} else if (down && lastTouchDown[i]) {
			// is down and was down = TOUCH MOVE
			if (touchedControl[i]) {
				context->game->luaBinder->screenControlPointerMove(touchedControl[i]->getName(), px, py);
			}
		} else if (!down && lastTouchDown[i]) {
			// is up and was down = TOUCH UP
			if (touchedControl[i]) {
				context->game->luaBinder->screenControlPointerUp(touchedControl[i]->getName(), px, py);
			}
			touchedControl[i] = NULL;
		}
		lastTouchDown[i] = down;
	}
}

void TouchInputProcessor::notifyScreenControlRemoved(ScreenControl *screenControl) {
	// we hold pointers to currently touched screen controls - calling this method notifies us that a control is going to be
	// deleted so we don't run into issues with pointing at the wrong memory address
	for (S32 i = 0; i < 10; i++) {
		if (touchedControl[i] == screenControl) {
			touchedControl[i] = NULL;
		}
	}
}
