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

#include "LuaBinder.h"
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/batterytech_globals.h>
#include <batterytech/Logger.h>
#include <batterytech/util/strx.h>
#include "../GameContext.h"
#include <batterytech/render/GLResourceManager.h>
#include <batterytech/render/GraphicsConfiguration.h>
#include "../GameUtil.h"
#include "../GameConstants.h"
#include "../gameobject/GameObject.h"
#include "../World.h"
#include "../gameobject/GameObjectLuaBinder.h"
#include "../render/Camera.h"
#include "../Game.h"
#include <batterytech/render/TextRasterRenderer.h>
#include "../render/WorldRenderer.h"
#include "../level/LevelIO.h"
#include "../menus/ErrorMenu.h"
#include <batterytech/VibrationManager.h>
#include <batterytech/video/VideoManager.h>

using namespace BatteryTech;

// c functions available to lua (LuaBinder should have all global functions)
static int lua_logmsg(lua_State *L);
static int lua_execScript(lua_State *L);
static int lua_loadAsset(lua_State *L);
static int lua_addFont(lua_State *L);
static int lua_addTexture(lua_State *L);
static int lua_removeTexture(lua_State *L);
static int lua_setDefaultTextureParams(lua_State *L);
static int lua_setTextureParams(lua_State *L);
static int lua_getTextureDimensions(lua_State *L);
static int lua_clearTextures(lua_State *L);
static int lua_addObj(lua_State *L);
static int lua_removeObj(lua_State *L);
static int lua_clearObjs(lua_State *L);
static int lua_addAssimp(lua_State *L);
static int lua_removeAssimp(lua_State *L);
static int lua_clearAssimps(lua_State *L);
static int lua_addSound(lua_State *L);
static int lua_removeSound(lua_State *L);
static int lua_playSound(lua_State *L);
static int lua_stopSound(lua_State *L);
static int lua_playStreamingSound(lua_State *L);
static int lua_stopStreamingSound(lua_State *L);
static int lua_setSoundRate(lua_State *L);
static int lua_getPointerState(lua_State *L);
static int lua_isUIConsumingPointers(lua_State *L);
static int lua_getAccelerometerState(lua_State *L);
static int lua_getLastCharacterTyped(lua_State *L);
static int lua_getKeyState(lua_State *L);
static int lua_isUIConsumingKeys(lua_State *L);
static int lua_getScreenCoord(lua_State *L);
static int lua_getCameraParams(lua_State *L);
static int lua_setCameraParams(lua_State *L);
static int lua_setCameraNearFarFOV(lua_State *L);
static int lua_zoomCamera(lua_State *L);
static int lua_panCamera(lua_State *L);
static int lua_getCameraLookVec(lua_State *L);
static int lua_getUIScale(lua_State *L);
static int lua_getScale2d(lua_State *L);
static int lua_getViewportSize(lua_State *L);
static int lua_panic(lua_State *L);
static int lua_getExternalStorageDirectoryName(lua_State *L);
static int lua_getApplicationStorageDirectoryName(lua_State *L);
static int lua_getPathSeparator(lua_State *L);
static int lua_pathExists(lua_State *L);
static int lua_showKeyboard(lua_State *L);
static int lua_hideKeyboard(lua_State* L);
static int lua_requestPurchase(lua_State *L);
static int lua_submitTime(lua_State *L);
static int lua_submitMPH(lua_State *L);
static int lua_submitLeaderboard(lua_State * L);
static int lua_unlockAchievement(lua_State *L);
static int lua_showAd(lua_State *L);
static int lua_showMessageDialog(lua_State *L);
static int lua_platformHook(lua_State *L);
static int lua_platformExit(lua_State *L);
static int lua_getTimeMillis(lua_State *L);
static int lua_setSoundEnabled(lua_State *L);
static int lua_setVibrationEnabled(lua_State *L);
static int lua_showFPS(lua_State *L);
static int lua_setShadowType(lua_State *L);
static int lua_loadVideo(lua_State *L);
static int lua_playVideo(lua_State *L);
static int lua_stopVideo(lua_State *L);
static int lua_unloadVideo(lua_State *L);
static int lua_pauseVideo(lua_State *L);
static int lua_getVideoPosition(lua_State *L);
static int lua_playVibrationEffect(lua_State *L);
static int lua_startVibrationEffect(lua_State *L);
static int lua_stopVibrationEffect(lua_State *L);
static int lua_stopAllVibrationEffects(lua_State *L);

static GameContext* static_context;

LuaBinder::LuaBinder(lua_State *L) {
	this->L = L;
}

LuaBinder::~LuaBinder() {
	L = NULL;
}

lua_State* LuaBinder::newState(GameContext *context) {
	static_context = context;
	lua_State* L = luaL_newstate();
	lua_atpanic(L, lua_panic);
	luaL_openlibs(L);
	registerFunction(L, "logmsg", lua_logmsg); // param: string message
	registerFunction(L, "execScript", lua_execScript); // param: string scriptAssetName
	registerFunction(L, "loadAsset", lua_loadAsset); // param: string assetName, returns asset contents, size
	registerFunction(L, "addFont", lua_addFont); // param: string tag, string pathToFont, float size, U32 color, int innerStroke, int outerStroke
	registerFunction(L, "addTexture", lua_addTexture); // param: string textureAssetName
	registerFunction(L, "removeTexture", lua_removeTexture); // param: string textureAssetName
	registerFunction(L, "setTextureParams", lua_setTextureParams); // param: string textureAssetName, multiple key/value pairs of parameters
    registerFunction(L, "setDefaultTextureParams", lua_setDefaultTextureParams); // params: multiple key/value pairs of parameters
	registerFunction(L, "getTextureDimensions", lua_getTextureDimensions); // param: string textureAssetName
	registerFunction(L, "clearTextures", lua_clearTextures);
	registerFunction(L, "addObj", lua_addObj); // param: string objAssetName
	registerFunction(L, "removeObj", lua_removeObj); // param: string objAssetName
	registerFunction(L, "clearObjs", lua_clearObjs);
	registerFunction(L, "addDynamic", lua_addAssimp); // param: string objAssetName
	registerFunction(L, "removeDynamic", lua_removeAssimp); // param: string objAssetName
	registerFunction(L, "clearDynamics", lua_clearAssimps);
	registerFunction(L, "addAssimp", lua_addAssimp); // param: string assimpAssetName
	registerFunction(L, "removeAssimp", lua_removeAssimp); // param: string assimpAssetName
	registerFunction(L, "clearAssimps", lua_clearAssimps);
	registerFunction(L, "addSound", lua_addSound);
	registerFunction(L, "removeSound", lua_removeSound);
	registerFunction(L, "playSound", lua_playSound);
	registerFunction(L, "stopSound", lua_stopSound);
	registerFunction(L, "playStreamingSound", lua_playStreamingSound);
	registerFunction(L, "stopStreamingSound", lua_stopStreamingSound);
	registerFunction(L, "setSoundRate", lua_setSoundRate);
	registerFunction(L, "getPointerState", lua_getPointerState); // param: int pointerIndex, return: bool isDown, xy screenCoordinate
	registerFunction(L, "getKeyState", lua_getKeyState); // param: int keyIndex, return: bool isDown, int keyCode
	registerFunction(L, "isUIConsumingPointers", lua_isUIConsumingPointers); // return: bool
	registerFunction(L, "getLastCharacterTyped", lua_getLastCharacterTyped); // return: char, specialChar enum
	registerFunction(L, "isUIConsumingKeys", lua_isUIConsumingKeys); // return: bool
	registerFunction(L, "getAccelerometerState", lua_getAccelerometerState); // xyz of accelerometer
	registerFunction(L, "getScreenCoord", lua_getScreenCoord); // param: xyz worldCoordinate, return: xyz screenCoordinate
	/*registerFunction(L, "pickObject", lua_pickObject); // param: xy screenCoordinate, slop, return: GameObject
	registerFunction(L, "pickObjects", lua_pickObjects); // param: xy screenCoordinate, slop, return: Multiple GameObjects
	registerFunction(L, "castRayToLevel", lua_castRayToLevel); // param: xy screenCoordinate, return: xyz
	registerFunction(L, "castRayToGameObject", lua_castRayToGameObject); // param: gameObject, xy screenCoordinate, return: xyz
	registerFunction(L, "getLineOfSightDistance", lua_getLineOfSightDistance); // param: xyz from, xyz to, GameObject exclusion1, GameObject exclusion2, return: distance
*/	registerFunction(L, "getCameraParams", lua_getCameraParams); // return: xyz, tilt, rotation
	registerFunction(L, "setCameraParams", lua_setCameraParams); // param: xyz, tilt, rotation
	registerFunction(L, "setCameraNearFarFOV", lua_setCameraNearFarFOV); // param: near, far, field of view
	registerFunction(L, "zoomCamera", lua_zoomCamera); // param: amount
	registerFunction(L, "panCamera", lua_panCamera); // param: amount
	registerFunction(L, "getCameraLookVec", lua_getCameraLookVec); // return: xyz
	registerFunction(L, "getUIScale", lua_getUIScale); // return: uiScale
	registerFunction(L, "getScale2d", lua_getScale2d); // return: scaleX2d, scaleY2d
	registerFunction(L, "getViewportSize", lua_getViewportSize); // return: scaleX2d, scaleY2d
	registerFunction(L, "getExternalStorageDirectoryName", lua_getExternalStorageDirectoryName);//return: string
	registerFunction(L, "getApplicationStorageDirectoryName", lua_getApplicationStorageDirectoryName);//return: string
	registerFunction(L, "getPathSeperator", lua_getPathSeparator);//returns: string // deprecated - use correct spelling version
	registerFunction(L, "getPathSeparator", lua_getPathSeparator);//returns: string
	registerFunction(L, "pathExists", lua_pathExists);//returns: boolean
	registerFunction(L, "showAd", lua_showAd);//param: boolean show
	registerFunction(L, "showKeyboard", lua_showKeyboard);
	registerFunction(L, "hideKeyboard", lua_hideKeyboard);
	registerFunction(L, "requestPurchase", lua_requestPurchase);//param: string productId, double price, string name, string desc
	registerFunction(L, "submitTime", lua_submitTime);//takes: float time, float top speed
	registerFunction(L, "submitMPH", lua_submitMPH);//takes: float time, float top speed
	registerFunction(L, "submitLeaderboard", lua_submitLeaderboard);//takes: board id, top distance
	registerFunction(L, "unlockAchievement", lua_unlockAchievement);//takes: achievementID
	registerFunction(L, "platformHook", lua_platformHook);// param: hook return: hookresult
	registerFunction(L, "showMessageDialog", lua_showMessageDialog); // param: title, message
	registerFunction(L, "platformExit", lua_platformExit); // param: title, message
	registerFunction(L, "getTimeMillis", lua_getTimeMillis); // return: time in ns
	registerFunction(L, "setSoundEnabled", lua_setSoundEnabled); // param: if sound is enabled
	registerFunction(L, "setVibrationEnabled", lua_setVibrationEnabled); // param: if vibration is enabled
	registerFunction(L, "showFPS", lua_showFPS); // param: if fps is to be shown
	registerFunction(L, "setShadowType", lua_setShadowType); // param: 0 = no shadow, 1 = low, 2 = high
	registerFunction(L, "loadVideo", lua_loadVideo); // param: assetName
	registerFunction(L, "playVideo", lua_playVideo);
	registerFunction(L, "stopVideo", lua_stopVideo);
	registerFunction(L, "unloadVideo", lua_unloadVideo);
	registerFunction(L, "pauseVideo", lua_pauseVideo);
	registerFunction(L, "getVideoPosition", lua_getVideoPosition);
	registerFunction(L, "playVibrationEffect", lua_playVibrationEffect);
	registerFunction(L, "startVibrationEffect", lua_startVibrationEffect);
	registerFunction(L, "stopVibrationEffect", lua_stopVibrationEffect);
	registerFunction(L, "stopAllVibrationEffects", lua_stopAllVibrationEffects);
	return L;
}

void LuaBinder::closeState(lua_State *L) {
	lua_close(L);
}

void LuaBinder::registerFunction(lua_State *L, const char *name, int (*func)(lua_State*) ) {
	lua_pushcfunction(L, func);
	lua_setglobal(L, name);
}

void LuaBinder::registerFunction(const char *name, int (*func)(lua_State*) ) {
	registerFunction(L, name, func);
}

// static
BOOL32 LuaBinder::execScript(lua_State *L, const char* scriptName, BOOL32 showNotFoundError, BOOL32 abspath) {
	char cBuf[255];
	sprintf(cBuf, "Executing Script: %s", scriptName);
	logmsg(cBuf);
	int bytesLoaded = 0;
	unsigned char* buf = NULL;
	if(abspath == FALSE) {
		buf = _platform_load_asset(scriptName, &bytesLoaded);
	} else {
		FILE* f = fopen(scriptName, "rb");
		if(f != NULL) {
			fseek(f, 0, SEEK_END);
			size_t length = ftell(f);
			buf = new unsigned char[length];
			fseek(f, 0, SEEK_SET);
			bytesLoaded = fread(buf, 1, length, f);
		} else {
			buf = NULL;
			bytesLoaded = 0;
		}
		fclose(f);
	}
	if (bytesLoaded == 0 && !buf) {
		if (showNotFoundError) {
			char buf[255];
			sprintf(buf, "Script not found: %s", scriptName);
			static_context->game->showError("Script not found", buf);
		}
		return FALSE;
	} else {
		int error = luaL_loadbuffer(L, (char*)buf, bytesLoaded, scriptName) || lua_pcall(L, 0, 0, 0);
		if (error) {
			char mBuf[255];
			sprintf(mBuf, "Executing Script %s: %s", scriptName, lua_tostring(L, -1));
			static_context->game->showError("Lua Error", mBuf);
			lua_pop(L, 1);
			return FALSE;
		}
	}
	return TRUE;
}

void LuaBinder::execScript(const char* scriptName) {
	execScript(L, scriptName, TRUE, FALSE);
}

char* LuaBinder::getGlobalString(const char* name) {
	char *string;
	lua_getglobal(L, name);
	if (lua_isstring(L, -1)) {
		string = strDuplicate(lua_tostring(L, -1));
	} else {
		string = NULL;
	}
	lua_pop(L, 1);
	return string;
}

char* LuaBinder::getStringFromGlobalTable(const char* tableName, const char* fieldName) {
	char* string = NULL;
	lua_getglobal(L, tableName);
	if(lua_istable(L, -1)) {
		lua_getfield(L, -1, fieldName);
		if(lua_isstring(L, -1)) {
			string = strDuplicate(lua_tostring(L, -1));
		} else {
			string = NULL;
		}
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	return string;
}

BOOL32 LuaBinder::getGlobalBool(const char* name) {
	BOOL32 result = FALSE;
	lua_getglobal(L, name);
	if (lua_isboolean(L, -1)) {
		result = lua_toboolean(L, -1);
	}
	lua_pop(L, 1);
	return result;
}

S32 LuaBinder::getGlobalInt(const char* name) {
	S32 result = 0;
	lua_getglobal(L, name);
	if (lua_isnumber(L, -1)) {
		result = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return result;
}

F32 LuaBinder::getGlobalFloat(const char* name) {
	F32 result = 0;
	lua_getglobal(L, name);
	if (lua_isnumber(L, -1)) {
		result = (F32)lua_tonumber(L, -1);
	}
	lua_pop(L, 1);
	return result;
}

// If withSelf, must have "self" at -1 and function at -2 in stack, otherwise function at -1
void LuaBinder::callFunction(const char *name, BOOL32 withSelf) {
	//stackDump();
	if (lua_pcall(L, withSelf, 0, 0) != 0)  /* do the call */ {
		char mBuf[255];
		sprintf(mBuf, "Function %s(): %s", name, lua_tostring(L, -1));
		static_context->game->showError("Lua Error", mBuf);
	}
}

// If withSelf, must have "self" at -1 and function at -2 in stack, otherwise function at -1
void LuaBinder::callFunctionVA(const char *name, BOOL32 withSelf, const char *sig, ...) {
	va_list vl;
	int narg, nres;  /* number of arguments and results */
	va_start(vl, sig);
	/* push arguments */
	narg = 0;
	while (*sig) {  /* push arguments */
		switch (*sig++) {
		  case 'd':  /* double argument */
			lua_pushnumber(L, va_arg(vl, F64));
			break;
		  case 'b':  /* boolean argument */
			lua_pushboolean(L, va_arg(vl, BOOL32));
			break;
		  case 'i':  /* int argument */
			lua_pushnumber(L, va_arg(vl, S32));
			break;
		  case 's':  /* string argument */
			lua_pushstring(L, va_arg(vl, char *));
			break;
		  case 'r':  /* registryindex argument */
			lua_rawgeti( L, LUA_REGISTRYINDEX, va_arg(vl, int) );
			break;
		  case '>':
			goto endwhile;
		  default:
			  logmsg("lua invalid va option");
			//error(L, "invalid option (%c)", *(sig - 1));
		}
		narg++;
		luaL_checkstack(L, 1, "too many arguments");
	} endwhile:
	/* do the call */
	nres = strlen(sig);  /* number of expected results */
	if (lua_pcall(L, narg + withSelf, nres, 0) != 0)  /* do the call */ {
		char mBuf[255];
		sprintf(mBuf, "Function %s(): %s", name, lua_tostring(L, -1));
		static_context->game->showError("Lua Error", mBuf);
	}
	/* retrieve results */
	nres = -nres;  /* stack index of first result */
	while (*sig) {  /* get results */
		switch (*sig++) {
		  case 'd':  /* double result */
			if (!lua_isnumber(L, nres)) {
			  //error(L, "wrong result type");
				logmsg("lua error wrong result type");
			}
			*va_arg(vl, F64 *) = lua_tonumber(L, nres);
			break;
		  case 'i':  /* int result */
			if (!lua_isnumber(L, nres)) {
				//error(L, "wrong result type");
				logmsg("lua error wrong result type");
			  }
			*va_arg(vl, S32 *) = (S32)lua_tonumber(L, nres);
			break;
		  case 'b':  /* boolean result */
			if (!lua_isboolean(L, nres)) {
				//error(L, "wrong result type");
				logmsg("lua error wrong result type");
			  }
			*va_arg(vl, BOOL32 *) = (BOOL32)lua_toboolean(L, nres);
			break;
		  case 's':  /* string result */
			if (!lua_isstring(L, nres)) {
				//error(L, "wrong result type");
				logmsg("lua error wrong result type");
			  }
			*va_arg(vl, const char **) = lua_tostring(L, nres);
			break;
		  default:
			  logmsg("lua invalid va option");
						//error(L, "invalid option (%c)", *(sig - 1));
		}
		nres++;
	}
	va_end(vl);
}

void LuaBinder::callGlobalVA (const char *func, const char *sig, ...) {
	va_list vl;
	int narg, nres;  /* number of arguments and results */
	va_start(vl, sig);
	lua_getglobal(L, func);  /* get function */
	if (lua_isnoneornil(L, -1)) {
	  lua_pop(L, 1);
	  va_end(vl);
	  return;
	}
	/* push arguments */
	narg = 0;
	while (*sig) {  /* push arguments */
		switch (*sig++) {
		  case 'b':  /* bool argument */
			lua_pushboolean(L, va_arg(vl, BOOL32));
			break;
		  case 'd':  /* double argument */
			lua_pushnumber(L, va_arg(vl, double));
			break;
		  case 'i':  /* int argument */
			lua_pushnumber(L, va_arg(vl, int));
			break;
		  case 's':  /* string argument */
			lua_pushstring(L, va_arg(vl, char *));
			break;
		  case '>':
			goto endwhile;
		  default:
			  logmsg("lua invalid va option");
			//error(L, "invalid option (%c)", *(sig - 1));
		}
		narg++;
		luaL_checkstack(L, 1, "too many arguments");
	} endwhile:
	/* do the call */
	nres = strlen(sig);  /* number of expected results */
	if (lua_pcall(L, narg, nres, 0) != 0)  /* do the call */ {
	//error(L, "error running function `%s': %s", func, lua_tostring(L, -1));
		char mBuf[255];
		sprintf(mBuf, "Function %s(): %s", func, lua_tostring(L, -1));
		static_context->game->showError("Lua Error", mBuf);
	}
	/* retrieve results */
	nres = -nres;  /* stack index of first result */
	while (*sig) {  /* get results */
		switch (*sig++) {
		  case 'b':  /* bool result */
			if (!lua_isboolean(L, nres)) {
			  //error(L, "wrong result type");
				logmsg("lua error wrong result type");
			}
			*va_arg(vl, BOOL32 *) = lua_toboolean(L, nres);
			break;
		  case 'd':  /* double result */
			if (!lua_isnumber(L, nres)) {
			  //error(L, "wrong result type");
				logmsg("lua error wrong result type");
			}
			*va_arg(vl, double *) = lua_tonumber(L, nres);
			break;
		  case 'i':  /* int result */
			if (!lua_isnumber(L, nres)) {
				//error(L, "wrong result type");
				logmsg("lua error wrong result type");
			  }
			*va_arg(vl, int *) = (int)lua_tonumber(L, nres);
			break;
		  case 's':  /* string result */
			if (!lua_isstring(L, nres)) {
				//error(L, "wrong result type");
				logmsg("lua error wrong result type");
			  }
			*va_arg(vl, const char **) = lua_tostring(L, nres);
			break;
		  default:
			  logmsg("lua invalid va option");
						//error(L, "invalid option (%c)", *(sig - 1));
		}
		nres++;
	}
	va_end(vl);
	//char mBuf[255];
	//sprintf(mBuf, "lua stack top at %d", lua_gettop(L));
	//logmsg(mBuf);
}

void LuaBinder::stackDump(lua_State *L) {
      int i;
      int top = lua_gettop(L);
      for (i = top; i >= 1; i--) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {
          case LUA_TSTRING:  /* strings */
            printf("`%s'", lua_tostring(L, i));
            break;
          case LUA_TBOOLEAN:  /* booleans */
            printf(lua_toboolean(L, i) ? "true" : "false");
            break;
          case LUA_TNUMBER:  /* numbers */
            printf("%g", lua_tonumber(L, i));
            break;
          default:  /* other values */
            printf("%s", lua_typename(L, t));
            break;
        }
        printf("  ");  /* put a separator */
      }
      printf("\n");  /* end the listing */
}

void LuaBinder::stackDump() {
	LuaBinder::stackDump(L);
}

static int lua_logmsg(lua_State *L) {
	const char *string = lua_tostring(L, 1);
	logmsg(string);
	return 0;
}

static int lua_execScript(lua_State *L) {
	const char *scriptName = lua_tostring(L, 1);
	BOOL32 showError = TRUE;
	BOOL32 abspath = FALSE;
	if (lua_isboolean(L, 2)) {
		showError = lua_toboolean(L, 2);
	}
	if(lua_isboolean(L, 3)) {
		abspath = lua_toboolean(L, 3);
	}
	BOOL32 success = LuaBinder::execScript(L, scriptName, showError, abspath);
	lua_pushboolean(L, success);
	return 1;
}

static int lua_loadAsset(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	char *data = _platform_load_text_asset(assetName);
	if (!data) {
		// error!
		return 0;
	} else {
		lua_pushstring(L, data);
		_platform_free_asset((unsigned char*)data);
		return 1;
	}
}

// param: string tag, string pathToFont, float size, U32 color, int innerStroke, int outerStroke
static int lua_addFont(lua_State *L) {
	const char *tag = lua_tostring(L, 1);
	const char *assetName = lua_tostring(L, 2);
	F32 size = lua_tonumber(L, 3);
	S32 innerStroke = lua_tointeger(L, 4);
	S32 outerStroke = lua_tointeger(L, 5);
	Vector4f color = Vector4f(1,1,1,1);
	if (lua_isnumber(L, 6) && lua_isnumber(L, 7) && lua_isnumber(L, 8) && lua_isnumber(L, 9)) {
		color = lua_toVector4f(L, 6) * 1/255.0f;
	}
	TextRasterRenderer *renderer = new TextRasterRenderer(static_context, assetName, size);
	renderer->setStroke(innerStroke, outerStroke);
	renderer->setColorFilter(color);
	static_context->worldRenderer->addTextRenderer(tag, renderer);
	return 0;
}

static int lua_addTexture(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	BOOL32 onDemand = FALSE;
	if (lua_toboolean(L, 2)) {
		onDemand = lua_toboolean(L, 2);
	}
	static_context->glResourceManager->addTexture(assetName, onDemand);
	return 0;
}

// pass in key/value pairs of texture parameters
static int lua_setTextureParams(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	if (assetName) {
		Texture *tex = static_context->glResourceManager->getTexture(assetName);
		if (tex) {
            // TODO - if texture is already loaded, live modify
			int n = 2;
			while (lua_isstring(L, n) && !lua_isnil(L, n+1)) {
				const char *paramName = lua_tostring(L, n);
				if (strEquals(paramName, "mipmap")) {
					tex->mipmap = lua_toboolean(L, n+1);
                } else if (strEquals(paramName, "filter")) {
                    // nearest, linear, bilinear, trilinear
                    const char *filter = lua_tostring(L, n+1);
					if (strEquals(filter, "nearest")) {
						tex->filter = GraphicsConfiguration::NONE;
					} else if (strEquals(filter, "linear")) {
						tex->filter = GraphicsConfiguration::LINEAR;
					} else if (strEquals(filter, "bilinear")) {
						tex->filter = GraphicsConfiguration::BILINEAR;
					} else if (strEquals(filter, "trilinear")) {
						tex->filter = GraphicsConfiguration::TRILINEAR;
					} else {
                        tex->filter = GraphicsConfiguration::DEFAULT;
                    }
				} else if (strEquals(paramName, "minfilter")) {
					const char *filter = lua_tostring(L, n+1);
					if (strEquals(filter, "nearest")) {
						tex->minFilter = Texture::TEX_FILTER_NEAREST;
					} else if (strEquals(filter, "linear")) {
						tex->minFilter = Texture::TEX_FILTER_LINEAR;
					} else {
                        tex->minFilter = Texture::TEX_FILTER_DEFAULT;
                    }
				} else if (strEquals(paramName, "magfilter")) {
					const char *filter = lua_tostring(L, n+1);
					if (strEquals(filter, "nearest")) {
						tex->magFilter = Texture::TEX_FILTER_NEAREST;
					} else if (strEquals(filter, "linear")) {
						tex->magFilter = Texture::TEX_FILTER_LINEAR;
					} else {
                        tex->minFilter = Texture::TEX_FILTER_DEFAULT;
                    }
				} else if (strEquals(paramName, "repeat")) {
                    BOOL32 on = lua_toboolean(L, n+1);
                    tex->repeatX = on;
                    tex->repeatY = on;
                } else if (strEquals(paramName, "repeatX")) {
                    BOOL32 on = lua_toboolean(L, n+1);
                    tex->repeatX = on;
                } else if (strEquals(paramName, "repeatY")) {
                    BOOL32 on = lua_toboolean(L, n+1);
                    tex->repeatY = on;
                }
				n += 2;
			}
		} else {
			char buf[1024];
			sprintf(buf, "lua_setTextureParams %s - texture not in resource manager", assetName);
			logmsg(buf);
		}
	}
	return 0;
}

static int lua_setDefaultTextureParams(lua_State *L) {
    int n = 1;
    while (lua_isstring(L, n) && !lua_isnil(L, n+1)) {
        const char *paramName = lua_tostring(L, n);
        if (strEquals(paramName, "filter")) {
            // nearest, linear, bilinear, trilinear
            const char *filter = lua_tostring(L, n+1);
            if (strEquals(filter, "nearest")) {
                static_context->gConfig->textureFilter = GraphicsConfiguration::NONE;
            } else if (strEquals(filter, "linear")) {
                static_context->gConfig->textureFilter = GraphicsConfiguration::LINEAR;
            } else if (strEquals(filter, "bilinear")) {
                static_context->gConfig->textureFilter = GraphicsConfiguration::BILINEAR;
            } else if (strEquals(filter, "trilinear")) {
                static_context->gConfig->textureFilter = GraphicsConfiguration::TRILINEAR;
            }
        } else if (strEquals(paramName, "repeat")) {
            BOOL32 on = lua_toboolean(L, n+1);
            static_context->gConfig->textureRepeat = on;
        }
        n += 2;
    }
    return 0;
}

static int lua_removeTexture(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	static_context->glResourceManager->removeTexture(assetName);
	return 0;
}

//returns width, height of the texture. nil,nil if the texture can't be found. 
static int lua_getTextureDimensions(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	if (assetName) {
		Texture* t = static_context->glResourceManager->getTexture(assetName);
		if(t == NULL) {
			lua_pushnil(L);
			lua_pushnil(L);
		} else {
			lua_pushinteger(L, t->getOriginalSize().x);
			lua_pushinteger(L, t->getOriginalSize().y);
			lua_pushinteger(L, t->getTrimmedSize().x);
			lua_pushinteger(L, t->getTrimmedSize().y);
			lua_pushinteger(L, t->getCornerOffset().x);
			lua_pushinteger(L, t->getCornerOffset().y);
			return 6;
		}
	} else {
		lua_pushnil(L);
		lua_pushnil(L);
	}
	return 2;
}

static int lua_clearTextures(lua_State *L) {
	static_context->glResourceManager->clearTextures();
	return 0;
}

static int lua_addObj(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	static_context->glResourceManager->addObjScene(assetName);
	return 0;
}

static int lua_removeObj(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	static_context->glResourceManager->removeObjScene(assetName);
	return 0;
}

static int lua_clearObjs(lua_State *L) {
	static_context->glResourceManager->clearObjScenes();
	return 0;
}

static int lua_addAssimp(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	static_context->glResourceManager->addAssimp(assetName);
	return 0;
}

static int lua_removeAssimp(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	static_context->glResourceManager->removeAssimp(assetName);
	return 0;
}

static int lua_clearAssimps(lua_State *L) {
	static_context->glResourceManager->clearAssimps();
	return 0;
}

static int lua_addSound(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	// default behavior is to load during loadNewResources()
	BOOL32 loadInstant = FALSE;
	if (lua_isboolean(L, 2)) {
		loadInstant = lua_toboolean(L, 2);
	}
	if (loadInstant) {
		static_context->audioManager->loadSound(assetName);
	} else {
		static_context->world->soundsPendingLoad->add(strDuplicate(assetName));
	}
	return 0;
}

static int lua_removeSound(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	static_context->audioManager->unloadSound(assetName);
	return 0;
}

static int lua_playSound(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	if (!assetName) {
		logmsg("playSound(nil) called - ignoring");
		lua_pushinteger(L, -1);
		return 1;
	}
	S16 loops = 0;
	F32 vol = 1.0f;
	F32 rate = 1.0f;
	if (lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4)) {
		loops = lua_tointeger(L, 2);
		vol = lua_tonumber(L, 3);
		rate = lua_tonumber(L, 4);
	}
	S32 streamId = static_context->audioManager->playSound(assetName, loops, vol, vol, rate);
	lua_pushinteger(L, streamId);
	return 1;
}

static int lua_stopSound(lua_State *L) {
	if (lua_isstring(L, 1) && !lua_isnumber(L, 1)) {
		const char *assetName = lua_tostring(L, 1);
		static_context->audioManager->stopSound(assetName);
	} else if (lua_isnumber(L, 1)) {
		S32 streamId = lua_tointeger(L, 1);
		static_context->audioManager->stopSound(streamId);
	}
	return 0;
}

static int lua_playStreamingSound(lua_State *L) {
	const char *assetName = lua_tostring(L, 1);
	S16 loops = 0;
	F32 vol = 1.0f;
	F32 rate = 1.0f;
	if (lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4)) {
		loops = lua_tointeger(L, 2);
		vol = lua_tonumber(L, 3);
		rate = lua_tonumber(L, 4);
	}
	static_context->audioManager->playStreamingSound(assetName, loops, vol, vol, rate);
	return 0;
}

static int lua_stopStreamingSound(lua_State *L) {
	if (lua_isstring(L, 1) && !lua_isnumber(L, 1)) {
		const char *assetName = lua_tostring(L, 1);
		static_context->audioManager->stopStreamingSound(assetName);
	}
	return 0;
}

static int lua_setSoundRate(lua_State *L) {
	S32 streamId = lua_tointeger(L, 1);
	F32 rate = lua_tonumber(L, 2);
	static_context->audioManager->setRate(streamId, rate);
	return 0;
}

static int lua_getPointerState(lua_State *L) {
	int pointerIndex = lua_tointeger(L, 1);
	if (pointerIndex < MAX_POINTERS) {
		lua_pushboolean(L, static_context->pointerState[pointerIndex].isDown);
		lua_pushnumber(L, static_context->pointerState[pointerIndex].x);
		lua_pushnumber(L, static_context->pointerState[pointerIndex].y);
		return 3;
	} else {
		return 0;
	}
}

static int lua_getKeyState(lua_State *L) {
	S32 idx = lua_tointeger(L, 1);
	if (idx < MAX_KEYSTATES) {
		lua_pushboolean(L, static_context->keyState[idx].isDown);
		lua_pushnumber(L, static_context->keyState[idx].keyCode);
		return 2;
	} else {
		return 0;
	}
}

static int lua_isUIConsumingPointers(lua_State *L) {
	lua_pushboolean(L, static_context->isUIConsumingPointers);
	return 1;
}

static int lua_isUIConsumingKeys(lua_State *L) {
	lua_pushboolean(L, static_context->isUIConsumingKeys);
	return 1;
}

static int lua_getLastCharacterTyped(lua_State *L) {
	if (static_context->keyPressed || static_context->specialKeyPressed != BatteryTech::SKEY_NULL) {
		lua_pushinteger(L, static_context->keyPressed);
		lua_pushinteger(L, static_context->specialKeyPressed);
		return 2;
	}
	return 0;
}

static int lua_getAccelerometerState(lua_State *L) {
	lua_pushnumber(L, static_context->accelerometerState.x);
	lua_pushnumber(L, static_context->accelerometerState.y);
	lua_pushnumber(L, static_context->accelerometerState.z);
	return 3;
}

static int lua_getScreenCoord(lua_State *L) {
	F32 x = lua_tonumber(L, 1);
	F32 y = lua_tonumber(L, 2);
	F32 z = lua_tonumber(L, 3);
	Vector3f screenCoord = GameUtil::getScreenCoord(Vector3f(x,y,z), static_context);
	lua_pushnumber(L, screenCoord.x);
	lua_pushnumber(L, screenCoord.y);
	lua_pushnumber(L, screenCoord.z);
	return 3;
}

/*static int lua_pickObject(lua_State *L) {
	F32 x = lua_tonumber(L, 1);
	F32 y = lua_tonumber(L, 2);
	F32 xySlop = lua_tonumber(L, 3);
	GameObject *gameObject = GameUtil::pickObject(Vector2f(x,y), static_context, xySlop);
	if (gameObject) {
		lua_rawgeti( L, LUA_REGISTRYINDEX, gameObject->luaBinder->luaRef);
		return 1;
	}
	return 0;
}

static int lua_pickObjects(lua_State *L) {
	F32 x = lua_tonumber(L, 1);
	F32 y = lua_tonumber(L, 2);
	F32 xySlop = lua_tonumber(L, 3);
	S32 count = 0;
	GameObject **gameObjects = GameUtil::pickObjects(Vector2f(x,y), static_context, xySlop, &count);
	for (S32 i = 0; i < count; i++) {
		lua_rawgeti( L, LUA_REGISTRYINDEX, gameObjects[i]->luaBinder->luaRef);
	}
	return count;
}

static int lua_castRayToLevel(lua_State *L) {
	F32 x = lua_tonumber(L, 1);
	F32 y = lua_tonumber(L, 2);
	Vector3f worldCoord = GameUtil::castRayToLevel(Vector2f(x,y), static_context);
	lua_pushnumber(L, worldCoord.x);
	lua_pushnumber(L, worldCoord.y);
	lua_pushnumber(L, worldCoord.z);
	return 3;
}

static int lua_castRayToGameObject(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	F32 x = lua_tonumber(L, 2);
	F32 y = lua_tonumber(L, 3);
	Vector3f intersection = GameUtil::castRayToGameObject(Vector2f(x,y), o, static_context);
	lua_pushnumber(L, intersection.x);
	lua_pushnumber(L, intersection.y);
	lua_pushnumber(L, intersection.z);
	return 3;
}

static int lua_getLineOfSightDistance(lua_State *L) {
	GameObject *exclusion1 = NULL;
	GameObject *exclusion2 = NULL;
	F32 ox = lua_tonumber(L, 1);
	F32 oy = lua_tonumber(L, 2);
	F32 oz = lua_tonumber(L, 3);
	F32 vx = lua_tonumber(L, 4);
	F32 vy = lua_tonumber(L, 5);
	F32 vz = lua_tonumber(L, 6);
	if (lua_isuserdata(L, 7)) {
		exclusion1 = *(GameObject**)lua_touserdata(L, 7);
	}
	if (lua_isuserdata(L, 8)) {
		exclusion2 = *(GameObject**)lua_touserdata(L, 8);
	}
	F32 dist = GameUtil::getLineOfSightDistance(Vector3f(ox,oy,oz), Vector3f(vx,vy,vz), static_context, exclusion1, exclusion2);
	lua_pushnumber(L, dist);
	return 1;
}*/

static int lua_getCameraParams(lua_State *L) {
	Camera *camera = static_context->world->camera;
	lua_pushnumber(L, camera->pos.x);
	lua_pushnumber(L, camera->pos.y);
	lua_pushnumber(L, camera->pos.z);
	lua_pushnumber(L, camera->tilt);
	lua_pushnumber(L, camera->rotation);
	return 5;
}

static int lua_setCameraParams(lua_State *L) {
	F32 x = lua_tonumber(L, 1);
	F32 y = lua_tonumber(L, 2);
	F32 z = lua_tonumber(L, 3);
	F32 tilt = lua_tonumber(L, 4);
	F32 rot = lua_tonumber(L, 5);
	Camera *camera = static_context->world->camera;
	camera->pos.x = x;
	camera->pos.y = y;
	camera->pos.z = z;
	camera->tilt = tilt;
	camera->rotation = rot;
	// update it so the new view vector is calculated
	camera->update();
	return 0;
}

static int lua_setCameraNearFarFOV(lua_State *L) {
	static_context->world->camera->setNearFarFOV(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	return 0;
}

static int lua_getCameraLookVec(lua_State *L) {
	Camera *camera = static_context->world->camera;
	lua_pushnumber(L, camera->look.x);
	lua_pushnumber(L, camera->look.y);
	lua_pushnumber(L, camera->look.z);
	return 3;
}

static int lua_zoomCamera(lua_State *L) {
	static_context->world->camera->zoom(lua_tonumber(L, 1));
	return 0;
}

static int lua_panCamera(lua_State *L) {
	static_context->world->camera->pan(lua_tonumber(L, 1), lua_tonumber(L, 2));
	return 0;
}

static int lua_getUIScale(lua_State *L) {
	lua_pushnumber(L, static_context->gConfig->uiScale);
	return 1;
}

static int lua_getScale2d(lua_State *L) {
	lua_pushnumber(L, static_context->gConfig->scaleX2d);
	lua_pushnumber(L, static_context->gConfig->scaleY2d);
	return 2;
}

static int lua_getViewportSize(lua_State *L) {
	lua_pushnumber(L, static_context->gConfig->viewportWidth);
	lua_pushnumber(L, static_context->gConfig->viewportHeight);
	return 2;
}

static int lua_panic(lua_State *L) {
	char buf[1024];
	sprintf(buf, "%s", lua_tostring(L, -1));
	static_context->game->showError("Lua Panic!", buf);
	return 0;
}

static int lua_getExternalStorageDirectoryName(lua_State *L) {
	char buf[1024];
	_platform_get_external_storage_dir_name(buf, 1024);
	lua_pushstring(L, buf);
	return 1;
}

static int lua_getApplicationStorageDirectoryName(lua_State *L) {
	char buf[1024];
	_platform_get_application_storage_dir_name(buf, 1024);
	lua_pushstring(L, buf);
	return 1;
}

static int lua_getPathSeparator(lua_State *L) {
	lua_pushstring(L, _platform_get_path_separator());
	return 1;
}

static int lua_pathExists(lua_State *L) {
	lua_pushboolean(L, _platform_path_exists(lua_tostring(L, 1)));
	return 1;
}

static int lua_showAd(lua_State *L) {
	BOOL32 show = TRUE;
	if (lua_isboolean(L, 1)) {
		show = lua_toboolean(L, 1);
	}
	if (show) {
		_platform_show_ad();
	} else {
		_platform_hide_ad();
	}
	return 0;
}

static int lua_showKeyboard(lua_State *L) {
	_platform_show_keyboard();
	return 0;
}

static int lua_hideKeyboard(lua_State *L) {
	_platform_hide_keyboard();
	return 0;
}

static int lua_requestPurchase(lua_State *L) {
	const char* productId = lua_tostring(L, 1);
	// double price = lua_tonumber(L, 2);
	// const char* name = lua_tostring(L, 3);
	// const char* desc = lua_tostring(L, 4);
	char hook[255];
	sprintf(hook, "requestPurchase %s", productId);
	_platform_hook(hook, NULL, 0);
	/*
	//for now just get the item to test the round trip
	lua_getglobal(L, "acquireItem");
	lua_pushstring(L, type);
	lua_pushstring(L, id);
	lua_pushboolean(L, TRUE);
	lua_call(L, 3, 0);
	*/
	return 0;
}

static int lua_submitTime(lua_State *L) {
	const char *leaderBoardID = lua_tostring(L, 1);
	F32 quarterMileTime = lua_tonumber(L, 2);
	S32 level = lua_tointeger(L, 3);
	char cBuf[255];
	sprintf(cBuf, "submitTime %s %f %d", leaderBoardID, quarterMileTime, level);
	logmsg(cBuf);
	_platform_hook(cBuf, NULL, 0);
	return 0;
}

static int lua_submitMPH(lua_State *L) {
	const char *leaderBoardID = lua_tostring(L, 1);
	F32 mph = lua_tonumber(L, 2);
	F32 time = lua_tonumber(L, 3);
	char cBuf[255];
	sprintf(cBuf, "submitMPH %s %f %f", leaderBoardID, mph, time);
	logmsg(cBuf);
	_platform_hook(cBuf, NULL, 0);
	return 0;
}

static int lua_submitLeaderboard(lua_State *L) {
	const char *leaderBoardID = lua_tostring(L, 1);
	F32 distance = lua_tonumber(L, 2);
	char cBuf[255];
	sprintf(cBuf, "submitLeaderboard %s %f", leaderBoardID, distance);
	logmsg(cBuf);
	_platform_hook(cBuf, NULL, 0);
	return 0;
}

static int lua_unlockAchievement(lua_State *L) {
	const char *achievementID = lua_tostring(L, 1);
	char cBuf[255];
	sprintf(cBuf, "unlockAchievement %s", achievementID);
	logmsg(cBuf);
	_platform_hook(cBuf, NULL, 0);
	return 0;
}

static int lua_platformHook(lua_State *L) {
	const char *hookString = lua_tostring(L, 1);
	char returnVal[1024];
	returnVal[0] = '\0';
	_platform_hook(hookString, returnVal, 1024);
	if (returnVal[0] != '\0') {
		lua_pushstring(L, returnVal);
		return 1;
	} else {
		return 0;
	}
}

static int lua_showMessageDialog(lua_State *L) {
	const char *title = lua_tostring(L, 1);
	const char *message = lua_tostring(L, 2);
	S32 width = 675;
	if (!lua_isnil(L, 3)) {
		width = lua_tointeger(L, 3);
	}
	ErrorMessage *msg = new ErrorMessage(strDuplicate(title), strDuplicate(message), width);
	msg->showRestart = false;
	static_context->uiManager->showMenu(ERROR_MENU_NAME, msg);
	return 0;
}

static int lua_platformExit(lua_State *L) {
	_platform_exit();
	return 0;
}

static int lua_getTimeMillis(lua_State *L) {
	lua_pushinteger(L, _platform_get_time_nanos() / 1000000);
	return 1;
}

static int lua_setSoundEnabled(lua_State *L) {
	static_context->audioManager->setEnabled(lua_toboolean(L, 1));
	return 0;
}

static int lua_setVibrationEnabled(lua_State *L) {
	static_context->vibrationManager->setEnabled(lua_toboolean(L, 1));
	return 0;
}

static int lua_showFPS(lua_State *L) {
	static_context->showFPS = lua_toboolean(L, 1);
	return 0;
}

static int lua_setShadowType(lua_State *L) {
	//static_context->gConfig->shadowType = lua_tointeger(L, 1);
	return 0;
}

static int lua_loadVideo(lua_State *L) {
	static_context->videoManager->load(lua_tostring(L, 1));
	return 0;
}

static int lua_playVideo(lua_State *L) {
	BOOL32 loop = FALSE;
	if (lua_isboolean(L, 1)) {
		loop = lua_toboolean(L, 1);
	}
	static_context->videoManager->play(loop);
	return 0;
}

static int lua_stopVideo(lua_State *L) {
	static_context->videoManager->stop();
	return 0;
}

static int lua_unloadVideo(lua_State *L) {
	static_context->videoManager->unload();
	return 0;
}

static int lua_pauseVideo(lua_State *L) {
	static_context->videoManager->pause();
	return 0;
}

static int lua_getVideoPosition(lua_State *L) {
	F32 pos = static_context->videoManager->getPosition();
	lua_pushnumber(L, pos);
	return 1;
}

static int lua_playVibrationEffect(lua_State *L) {
	S32 effectId = lua_tointeger(L, 1);
	F32 intensity = 1.0f;
	if (lua_isnumber(L, 2)) {
		intensity = lua_tonumber(L, 2);
	}
	static_context->vibrationManager->playEffect(effectId, intensity);
    return 0;
}

static int lua_startVibrationEffect(lua_State *L) {
	S32 effectId = lua_tointeger(L, 1);
	F32 intensity = 1.0f;
	if (lua_isnumber(L, 2)) {
		intensity = lua_tonumber(L, 2);
	}
	static_context->vibrationManager->startEffect(effectId, intensity);
    return 0;
}

static int lua_stopVibrationEffect(lua_State *L) {
	S32 effectId = lua_tointeger(L, 1);
	static_context->vibrationManager->stopEffect(effectId);
    return 0;
}

static int lua_stopAllVibrationEffects(lua_State *L) {
	static_context->vibrationManager->stopAllEffects();
    return 0;
}

Vector2f lua_toVector2f(lua_State *L, S32 startIdx) {
	return Vector2f(lua_tonumber(L, startIdx), lua_tonumber(L, startIdx+1));
}

Vector3f lua_toVector3f(lua_State *L, S32 startIdx) {
	return Vector3f(lua_tonumber(L, startIdx), lua_tonumber(L, startIdx+1), lua_tonumber(L, startIdx+2));
}

Vector4f lua_toVector4f(lua_State *L, S32 startIdx) {
	return Vector4f(lua_tonumber(L, startIdx), lua_tonumber(L, startIdx+1), lua_tonumber(L, startIdx+2), lua_tonumber(L, startIdx+3));
}

Vector4i lua_toVector4i(lua_State *L, S32 startIdx) {
	return Vector4i(lua_tointeger(L, startIdx), lua_tointeger(L, startIdx+1), lua_tointeger(L, startIdx+2), lua_tointeger(L, startIdx+3));
}

Matrix4f lua_toMatrix4f(lua_State *L, S32 startIdx) {
	return Matrix4f(lua_tonumber(L, startIdx), lua_tonumber(L, startIdx+1), lua_tonumber(L, startIdx+2), lua_tonumber(L, startIdx+3),
			lua_tonumber(L, startIdx+4), lua_tonumber(L, startIdx+5), lua_tonumber(L, startIdx+6), lua_tonumber(L, startIdx+7),
			lua_tonumber(L, startIdx+8), lua_tonumber(L, startIdx+9), lua_tonumber(L, startIdx+10), lua_tonumber(L, startIdx+11),
			lua_tonumber(L, startIdx+12), lua_tonumber(L, startIdx+13), lua_tonumber(L, startIdx+14), lua_tonumber(L, startIdx+15));
}
