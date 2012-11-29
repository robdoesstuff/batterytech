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

#ifndef GAMELUABINDER_H_
#define GAMELUABINDER_H_

#include "script/LuaBinder.h"
#include <batterytech/primitives.h>
#include <batterytech/Logger.h>
#ifdef BATTERYTECH_INCLUDE_BOX2D
#include <bt-box2d/Dynamics/b2Fixture.h>
#include <bt-box2d/Dynamics/b2Body.h>
#include <bt-box2d/Dynamics/b2WorldCallbacks.h>
#include "PhysicsBodyObject.h"
#endif

class Game;
class GameObject;
class GameContext;

using namespace BatteryTech;

#ifdef BATTERYTECH_INCLUDE_BOX2D
class LuaBinderQueryCallback : public b2QueryCallback {
public:
	LuaBinderQueryCallback(lua_State *L) {
		this->L = L;
		returnCount = 0;
	}
    bool ReportFixture(b2Fixture* fixture);
    lua_State *L;
    S32 returnCount;
};
#endif

struct GameEditorObject {
	char *name;
	char *className;
	virtual ~GameEditorObject() {
		logmsg("Deleting editor object");
		delete [] name;
		delete [] className;
	}
};

class GameLuaBinder : public LuaBinder {
public:
	GameLuaBinder(lua_State *L, Game *game, GameContext *context);
	virtual ~GameLuaBinder();
	// registration
	static void luaRegister(lua_State *L, GameContext *context);
	// lua functions
	// The string is allocated using new.. you must delete [] it when finished.
	char* getString(const char* name);
	BOOL32 getBool(const char* name);
	S32 getInt(const char* name);
	F32 getFloat(const char* name);
	void init();
	void update();
	void render(S32 state);
	void reset(BOOL32 invalidate);
	void setMode(S32 mode);
	void screenControlPointerDown(const char *name, F32 x, F32 y);
	void screenControlPointerMove(const char *name, F32 x, F32 y);
	void screenControlPointerUp(const char *name, F32 x, F32 y);
	// level editor support
	GameEditorObject* getEditorObjects(S32 *count);
	void editorCopyObject(GameObject *obj);
	void editorAddObject(const char *className);
	void editorSave();
	void editorEnable(BOOL32 enabled);
	void editorFreelook(BOOL32 enabled);
	void editorDelete(GameObject *obj);
	void editorUseCurrentTransform(GameObject *obj);
	// gameRef should be private but needs to be accessible to static functions.
	S32 gameRef;
protected:
	BOOL32 pushInstanceFunction(const char *name);
private:
	Game *game;
	GameContext *context;
};

#endif /* GAMELUABINDER_H_ */
