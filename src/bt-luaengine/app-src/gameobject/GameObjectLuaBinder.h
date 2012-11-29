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

#ifndef GAMEOBJECTLUABINDER_H_
#define GAMEOBJECTLUABINDER_H_

#include "../script/LuaBinder.h"
#include <batterytech/primitives.h>
#include <batterytech/util/ManagedArray.h>

class GameObject;
class GameContext;

using namespace BatteryTech;

struct GameObjectProperty {
	enum PropertyType{PROP_TYPE_INTEGER, PROP_TYPE_NUMBER, PROP_TYPE_BOOLEAN, PROP_TYPE_STRING};
	char *name;
	char *stringValue;
	PropertyType pType;
	GameObjectProperty() { name=NULL; stringValue=NULL; pType=PROP_TYPE_INTEGER;}
	virtual ~GameObjectProperty() {
		delete [] name;
		name = NULL;
		delete [] stringValue;
		stringValue = NULL;
	}
};

class GameObjectLuaBinder : public LuaBinder {
public:
	GameObjectLuaBinder(lua_State *L, GameObject *gameObject, GameContext *context);
	virtual ~GameObjectLuaBinder();
	// registration
	static void luaRegister(lua_State *L, GameContext *context);
	// lua functions
	void init(); //unused
	void update();
	void onCollisionStarted(GameObject *other, F32 force, F32 approachVelocity);
	void onCollisionUpdated(GameObject *other, F32 force, F32 approachVelocity);
	void onCollisionEnded(GameObject *other);
	void onLevelCollision();
	void onLevelCollisionEnded();
	// The string is allocated using new.. you must delete [] it when finished.
	char* getString(const char* name);
	BOOL32 getBool(const char* name);
	S32 getInt(const char* name);
	F32 getFloat(const char* name);
	ManagedArray<GameObjectProperty>* getProperties();
	void setPropertyS32(const char* key, S32 value);
	void setPropertyF32(const char* key, F32 value);
	void setPropertyBool32(const char* key, BOOL32 value);
	void setPropertyString(const char* key, const char* value);
	// gameRef should be private but needs to be accessible to static functions.
	S32 luaRef;
	BOOL32 pushInstanceFunction(const char *name);
private:
	GameObject *gameObject;
	GameContext *context;
};

#endif /* GAMEOBJECTLUABINDER_H_ */
