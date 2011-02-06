/*
 * World.h
 *
 *  Created on: Sep 4, 2010
 *      Author: rgreen
 */

#ifndef WORLD_H_
#define WORLD_H_

#include <batterytech/primitives.h>
#include <Box2D/Dynamics/b2World.h>
#include <batterytech/util/ManagedArray.h>
#include <batterytech/util/Triangulator.h>
#include "gameobject/GameObject.h"

class GameObject;

class World {
public:
	World();
	virtual ~World();
	void clear();

	BOOL32 down1;
	BOOL32 down2;
	S32 x1,y1;
	S32 x2,y2;
	F32 physicsTimeRemainder;
	F32 tickDelta;
	BOOL32 leftFlipperPressed;
	BOOL32 rightFlipperPressed;

	b2World *boxWorld;
	U8 gameState;
	U8 lastGameState;
	ManagedArray<GameObject> *pinballs;

	U32 score;
	S32 ballsRemaining;
	S32 level;
	BOOL32 levelLoaded;
	BOOL32 renderersReady;
};

#endif /* WORLD_H_ */
