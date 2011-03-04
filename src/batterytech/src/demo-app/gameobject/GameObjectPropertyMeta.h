/*
 * GameObjectPropertyMeta.h
 *
 *  Created on: Dec 12, 2010
 *      Author: rgreen
 */

#ifndef GAMEOBJECTPROPERTYMETA_H_
#define GAMEOBJECTPROPERTYMETA_H_

#include <batterytech/primitives.h>
#include <batterytech/util/ManagedArray.h>

using BatteryTech::ManagedArray;

class GameObjectPropertyMeta {
public:
	GameObjectPropertyMeta(const char* name, ManagedArray<const char> *choices);
	virtual ~GameObjectPropertyMeta();
	const char* name;
	ManagedArray<const char> *choices;
};

#endif /* GAMEOBJECTPROPERTYMETA_H_ */
