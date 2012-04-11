/*
 * GameObjectFactory.h
 *
 *  Created on: Nov 26, 2010
 *      Author: rgreen
 */

#ifndef GAMEOBJECTFACTORY_H_
#define GAMEOBJECTFACTORY_H_

#include <batterytech/primitives.h>
#include "../GameContext.h"
#include "GameObject.h"
#include "GameObjectMeta.h"
#include <batterytech/math/Vec2f.h>

using namespace BatteryTech;

class GameObjectFactory {
public:
	GameObjectFactory();
	virtual ~GameObjectFactory();
	static GameObject* createGameObject(GameContext *context, S32 objectType, Vec2f *points);
};

#endif /* GAMEOBJECTFACTORY_H_ */
