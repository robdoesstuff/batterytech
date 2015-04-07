/*
 * World.h
 *
 *  Created on: Sep 4, 2010
 *      Author: rgreen
 */

#ifndef WORLD_H_
#define WORLD_H_

#include <batterytech/primitives.h>
#include <bt-box2d/Dynamics/b2World.h>
#include <batterytech/util/ManagedArray.h>
#include <batterytech/math/Triangulator.h>
#include "gameobject/GameObject.h"

class GameObject;

class World {
public:
	World();
	virtual ~World();
	void clear();

	F32 physicsTimeRemainder;

	b2World *boxWorld;
	U8 gameState;
	U8 lastGameState;
	ManagedArray<GameObject> *gameObjects;

	BOOL32 levelLoaded;
	BOOL32 renderersReady;
};

#endif /* WORLD_H_ */
