/*
 * GameObjectFactory.cpp
 *
 *  Created on: Nov 26, 2010
 *      Author: rgreen
 */

#include "GameObjectFactory.h"
#include "../GameConstants.h"
#include "../gameobjects/Ball.h"

GameObjectFactory::GameObjectFactory() {
}

GameObjectFactory::~GameObjectFactory() {
}

/* Creates an initialized object - used when creating in editor */
GameObject* GameObjectFactory::createGameObject(GameContext *context, S32 objectType, Vector2f *points) {
	if (objectType == GAMEOBJECT_TYPE_BALL) {
		return new Ball(context, points[0].x, points[0].y, 0);
	}
	return NULL;
}
