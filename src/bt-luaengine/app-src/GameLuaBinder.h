/*
 * GameLuaBinder.h
 *
 *  Created on: May 5, 2011
 *      Author: rgreen
 */

#ifndef GAMELUABINDER_H_
#define GAMELUABINDER_H_

#include "script/LuaBinder.h"
#include <batterytech/primitives.h>
#include <batterytech/Logger.h>

class Game;
class GameObject;
class GameContext;

using namespace BatteryTech;

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
