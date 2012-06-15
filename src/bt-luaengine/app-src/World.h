/*
 * World.h
 *
 *  Created on: Sep 4, 2010
 *      Author: rgreen
 */

#ifndef WORLD_H_
#define WORLD_H_

#include <batterytech/primitives.h>
#include <batterytech/util/ManagedArray.h>
#include "render/Camera.h"
#include <batterytech/math/Vector3.h>
extern "C" {
	#include <bt-lua/lua.h>
}

class GameObject;
class Level;
class ScreenControl;
class RenderItem;
class GlobalLight;
class LocalLight;
class ParticleEmitter;

class World {
public:
	World();
	virtual ~World();
	void clear();

	lua_State *luaState;

	char boxMessage[255];
	S32 physicsUpdateTime;

	//btClock *btProfiler;
	//btDynamicsWorld *btWorld;
	BOOL32 wfMode;
	Level *level;
	S32 levelNumber;

	Camera *camera;
	//configured from lua setGlobalLight...
	GlobalLight *globalLight;
	LocalLight *localLights;
	S32 localLightsUsed;

	U8 gameState;
	U8 lastGameState;
	U8 nextGameState;
	BOOL32 simEnabled;
	ManagedArray<GameObject> *gameObjects;
	ManagedArray<ScreenControl> *screenControls;
	HashTable<S32, ParticleEmitter*> *emitters;
	RenderItem *renderItems;
	S32 renderItemsUsed;

	BOOL32 levelLoaded;
	BOOL32 renderersReady;
};

#endif /* WORLD_H_ */
