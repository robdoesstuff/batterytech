/*
 * World.cpp
 *
 *  Created on: Sep 4, 2010
 *      Author: rgreen
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
#endif
	gameObjects = new ManagedArray<GameObject>(MAX_GAMEOBJECTS);
	screenControls = new ManagedArray<ScreenControl>(MAX_SCREENCONTROLS);
	renderItems = new RenderItem[MAX_RENDERITEMS]; // this is going to use a lot of memory.
	renderItemsUsed = 0;
	localLights = new LocalLight[MAX_LOCALLIGHTS];
	localLightsUsed = 0;
	globalLight = new GlobalLight;
	emitters = new HashTable<S32, ParticleEmitter*>(MAX_PARTICLE_EMITTER);
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
	delete camera;
	camera = NULL;
	delete globalLight;
	globalLight = NULL;
	delete [] localLights;
	localLights = NULL;
	emitters->deleteElements();
	delete emitters;
	emitters = NULL;
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
