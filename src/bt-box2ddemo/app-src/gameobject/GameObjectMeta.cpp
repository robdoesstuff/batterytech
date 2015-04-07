/*
 * GameObjectMeta.cpp
 *
 *  Created on: Nov 29, 2010
 *      Author: rgreen
 */

#include "GameObjectMeta.h"

GameObjectMeta::GameObjectMeta(S32 placementPointCount) {
	this->placementPointCount = placementPointCount;
	properties = new ManagedArray<GameObjectPropertyMeta>(10);
}

GameObjectMeta::~GameObjectMeta() {
	if (properties) {
		properties->deleteElements();
		delete properties;
	}
	properties = NULL;
}

S32 GameObjectMeta::getPlacementPointCount() {
	return placementPointCount;
}

void GameObjectMeta::addProperty(GameObjectPropertyMeta *propertyMeta) {
	properties->add(propertyMeta);
}

ManagedArray<GameObjectPropertyMeta>* GameObjectMeta::getProperties() {
	return properties;
}
