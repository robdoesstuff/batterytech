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

#ifndef WORLD_H_
#define WORLD_H_

#include <batterytech/primitives.h>
#include <batterytech/util/ManagedArray.h>
#include "render/Camera.h"
#include <batterytech/math/Vector3.h>
#ifdef BATTERYTECH_INCLUDE_BOX2D
#include <bt-box2d/Dynamics/b2World.h>
#endif
#ifdef BATTERYTECH_INCLUDE_BULLET
#endif
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

	S32 physicsUpdateTime;

#ifdef BATTERYTECH_INCLUDE_BULLET
	btClock *btProfiler;
	btDynamicsWorld *btWorld;
#endif
#ifdef BATTERYTECH_INCLUDE_BOX2D
    b2World *boxWorld;
    HashTable<S32, b2Joint*> *boxJoints;
#endif
	BOOL32 wfMode;
	Level *level;

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
	ManagedArray<char> *soundsPendingLoad;
	RenderItem *renderItems;
	S32 renderItemsUsed;

	BOOL32 levelLoaded;
	BOOL32 renderersReady;
	BOOL32 physicsDrawDebug;
    BOOL32 physicsDrawDebugUsingProjection;
    // match glOrtho params
    F32 physicsDrawDebugProjection[6];
};

#endif /* WORLD_H_ */
