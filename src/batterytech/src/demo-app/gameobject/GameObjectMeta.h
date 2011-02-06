/*
 * GameObjectMeta.h
 *
 *  Created on: Nov 29, 2010
 *      Author: rgreen
 */

#ifndef GAMEOBJECTMETA_H_
#define GAMEOBJECTMETA_H_

#include <batterytech/primitives.h>
#include <batterytech/util/ManagedArray.h>
#include "GameObjectPropertyMeta.h"

class GameObjectMeta {
public:
	GameObjectMeta(S32 placementPointCount);
	virtual ~GameObjectMeta();
	virtual S32 getPlacementPointCount();
	void addProperty(GameObjectPropertyMeta *propertyMeta);
	ManagedArray<GameObjectPropertyMeta>* getProperties();
private:
	S32 placementPointCount;
	ManagedArray<GameObjectPropertyMeta> *properties;
};

#endif /* GAMEOBJECTMETA_H_ */
