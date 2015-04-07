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

#include "World.h"
#include "GameConstants.h"
#include <batterytech/primitives.h>
#include <batterytech/ui/UIComponent.h>
#include "level/Level.h"
#include "script/LuaBinder.h"
#include "ScreenControl.h"
#include "render/RenderItem.h"
#include "render/GlobalLight.h"
#include "render/LocalLight.h"
#include "gameobject/GameObject.h"
#include "gameobject/ParticleEmitter.h"

World::World() {
#ifdef BATTERYTECH_INCLUDE_BULLET
	btWorld = NULL;
	btProfiler = new btClock();
#endif
#ifdef BATTERYTECH_INCLUDE_BOX2D
    boxWorld = NULL;
    boxJoints = new HashTable<S32, b2Joint*>(MAX_JOINTS*1.3f);
#endif
	gameObjects = new ManagedArray<GameObject>(MAX_GAMEOBJECTS);
	screenControls = new ManagedArray<ScreenControl>(MAX_SCREENCONTROLS);
	renderItems = new RenderItem[MAX_RENDERITEMS]; // this is going to use a lot of memory.
	renderItemsUsed = 0;
	localLights = new LocalLight[MAX_LOCALLIGHTS];
	localLightsUsed = 0;
	globalLight = new GlobalLight;
	emitters = new HashTable<S32, ParticleEmitter*>(MAX_PARTICLE_EMITTER);
	soundsPendingLoad = new ManagedArray<char>(MAX_SOUND_LOAD_PENDING);
	physicsUpdateTime = 0;
	levelLoaded = FALSE;
	renderersReady = FALSE;
	wfMode = FALSE;
	luaState = NULL;
	level = NULL;
	simEnabled = TRUE;
	gameState = 0;
	lastGameState = 0;
	nextGameState = 0;
	physicsDrawDebug = FALSE;
    physicsDrawDebugUsingProjection = FALSE;
}

World::~World() {
	logmsg("Releasing World");
	clear();
	if (gameObjects) {
		delete gameObjects;
	}
	gameObjects = NULL;
	if (screenControls) {
		screenControls->deleteElements();
		delete screenControls;
	}
	screenControls = NULL;
#ifdef BATTERYTECH_INCLUDE_BULLET
	delete btProfiler;
	btProfiler = NULL;
#endif
#ifdef BATTERYTECH_INCLUDE_BOX2D
    delete boxJoints;
    boxJoints = NULL;
    if (boxWorld) {
		delete boxWorld;
	}
	boxWorld = NULL;
#endif
	delete camera;
	camera = NULL;
	delete globalLight;
	globalLight = NULL;
	delete [] localLights;
	localLights = NULL;
	emitters->deleteElements();
	delete emitters;
	emitters = NULL;
	soundsPendingLoad->deleteElements();
	delete soundsPendingLoad;
	soundsPendingLoad = NULL;
	if (luaState) {
		LuaBinder::closeState(luaState);
	}
	luaState = NULL;
}

void World::clear() {
	logmsg("Clearing game objects");
	if (gameObjects) {
		for (S32 i = 0; i < gameObjects->getSize(); i++) {
			delete gameObjects->array[i];
		}
		gameObjects->clear();
	}
#ifdef BATTERYTECH_INCLUDE_BULLET
	 logmsg("Clearing bullet world");
	if (btWorld) {
		delete btWorld;
	}
	btWorld = NULL;
#endif
	if (level) {
		delete level;
	}
	level = NULL;
#ifdef BATTERYTECH_INCLUDE_BOX2D
	logmsg("Clearing box world");
	// Delete boxworld LAST (or crashes occur when freeing other objects)
	// (it automatically deallocates everything internally)
	if (boxWorld) {
		delete boxWorld;
	}
	boxWorld = NULL;
#endif
}
