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

World::World() {
	//btWorld = NULL;
	//btProfiler = new btClock();
	gameObjects = new ManagedArray<GameObject>(MAX_GAMEOBJECTS);
	screenControls = new ManagedArray<ScreenControl>(MAX_SCREENCONTROLS);
	renderItems = new RenderItem[MAX_RENDERITEMS]; // this is going to use a lot of memory.
	renderItemsUsed = 0;
	localLights = new LocalLight[MAX_LOCALLIGHTS];
	localLightsUsed = 0;
	globalLight = new GlobalLight;

	physicsUpdateTime = 0;
	levelLoaded = FALSE;
	renderersReady = FALSE;
	wfMode = FALSE;
	luaState = NULL;
	boxMessage[0] = '\0';
	level = NULL;
	levelNumber = 0;
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
	//delete btProfiler;
	//btProfiler = NULL;
	delete camera;
	camera = NULL;
	delete globalLight;
	globalLight = NULL;
	delete [] localLights;
	localLights = NULL;
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
	logmsg("Clearing bullet world");
	//if (btWorld) {
	//	delete btWorld;
	//}
	//btWorld = NULL;
	if (level) {
		delete level;
	}
	level = NULL;
}
