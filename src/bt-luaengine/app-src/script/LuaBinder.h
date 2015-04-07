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

#ifndef LUABINDER_H_
#define LUABINDER_H_

#include <batterytech/primitives.h>
extern "C" {
	#include <bt-lua/lua.h>
	#include <bt-lua/lauxlib.h>
	#include <bt-lua/lualib.h>
}

#include <batterytech/math/Vector2.h>
#include <batterytech/math/Vector3.h>
#include <batterytech/math/Vector4.h>
#include <batterytech/math/Matrix4.h>

class GameContext;
using namespace BatteryTech;

Vector2f lua_toVector2f(lua_State *L, S32 startIdx);
Vector3f lua_toVector3f(lua_State *L, S32 startIdx);
Vector4f lua_toVector4f(lua_State *L, S32 startIdx);
Vector4i lua_toVector4i(lua_State *L, S32 startIdx);
Matrix4f lua_toMatrix4f(lua_State *L, S32 startIdx);

class LuaBinder {
public:
	LuaBinder(lua_State *L);
	virtual ~LuaBinder();
	static lua_State* newState(GameContext *context);
	static void closeState(lua_State *L);
	static BOOL32 execScript(lua_State *L, const char* scriptName, BOOL32 showNotFoundError, BOOL32 abspath);
	static void stackDump(lua_State *L);
	void execScript(const char* scriptName);
	// The string is allocated using new.. you must delete [] it when finished.
	char* getGlobalString(const char* name);
	char* getStringFromGlobalTable(const char* tableName, const char* fieldName);
	BOOL32 getGlobalBool(const char* name);
	S32 getGlobalInt(const char* name);
	F32 getGlobalFloat(const char* name);
	void callGlobalVA(const char *name, const char *sig, ...);
	lua_State *L;
	void callFunction(const char *name, BOOL32 withSelf);
	void callFunctionVA(const char *name, BOOL32 withSelf, const char *sig, ...);
protected:
	static void registerFunction(lua_State *L, const char *name, int (*func)(lua_State*));
	void registerFunction(const char *name, int (*func)(lua_State*));
	void stackDump();
};

#endif /* GAMELUABINDER_H_ */
