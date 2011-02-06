/*
 * GameObjectFactory.h
 *
 *  Created on: Nov 26, 2010
 *      Author: rgreen
 */

#ifndef GAMEOBJECTFACTORY_H_
#define GAMEOBJECTFACTORY_H_

#include <batterytech/primitives.h>
#include <batterytech/Context.h>
#include "GameObject.h"
#include "GameObjectMeta.h"
#include <batterytech/util/tvec2d.h>

class GameObjectFactory {
public:
	GameObjectFactory();
	virtual ~GameObjectFactory();
	static GameObject* createGameObject(Context *context, S32 objectType, TVEC2D *points);
};

#endif /* GAMEOBJECTFACTORY_H_ */
