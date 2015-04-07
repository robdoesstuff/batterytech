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

World::World() {
	boxWorld = NULL;
	gameObjects = new ManagedArray<GameObject>(MAX_GAMEOBJECTS);
	physicsTimeRemainder = 0.0f;
	levelLoaded = FALSE;
	renderersReady = FALSE;
}

World::~World() {
	logmsg("Releasing World");
	clear();
	if (gameObjects) {
		delete gameObjects;
	}
}

void World::clear() {
	logmsg("Clearing game objects");
	if (gameObjects) {
		gameObjects->deleteElements();
	}
	logmsg("Clearing box world");
	// Delete boxworld LAST (or crashes occur when freeing other objects)
	// (it automatically deallocates everything internally)
	if (boxWorld) {
		delete boxWorld;
	}
	boxWorld = NULL;

}
