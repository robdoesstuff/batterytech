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
	tickDelta = 0;
	down1 = FALSE;
	down2 = FALSE;
	boxWorld = NULL;
	pinballs = new ManagedArray<GameObject>(MAX_PINBALLS);
	leftFlipperPressed = FALSE;
	rightFlipperPressed = FALSE;
	physicsTimeRemainder = 0.0f;
	score = 0;
	ballsRemaining = 0;
	level = 0;
	levelLoaded = FALSE;
	renderersReady = FALSE;
}

World::~World() {
	logmsg("Releasing World");
	clear();
	if (pinballs) {
		delete pinballs;
	}
}

void World::clear() {
	logmsg("Clearing pinballs");
	if (pinballs) {
		pinballs->deleteElements();
	}
	logmsg("Clearing box world");
	// Delete boxworld LAST (or crashes occur when freeing other objects)
	// (it automatically deallocates everything internally)
	if (boxWorld) {
		delete boxWorld;
	}
	boxWorld = NULL;

}
