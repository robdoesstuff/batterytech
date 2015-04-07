/*
 * GameObjectPropertyMeta.cpp
 *
 *  Created on: Dec 12, 2010
 *      Author: rgreen
 */

#include "GameObjectPropertyMeta.h"
#include <batterytech/primitives.h>

GameObjectPropertyMeta::GameObjectPropertyMeta(const char* name, ManagedArray<const char> *choices) {
	this->name = name;
	this->choices = choices;
}

GameObjectPropertyMeta::~GameObjectPropertyMeta() {
	if (choices) {
		// do not delete const char* as they are probably static.  TODO - use safe copies.
		//choices->deleteElements();
		delete choices;
	}
	choices = NULL;
	name = NULL;
}
