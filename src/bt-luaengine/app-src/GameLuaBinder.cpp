/*
 * GameLuaBinder.cpp
 *
 *  Created on: May 5, 2011
 *      Author: rgreen
 */

#include "GameLuaBinder.h"
#include "Game.h"
#include "GameContext.h"
#include <batterytech/Logger.h>
#include "World.h"
#include "ScreenControl.h"
#include <batterytech/util/strx.h>
#include "level/Level.h"
#include <batterytech/ui/UIManager.h>
#include "render/RenderItem.h"
#include "gameobject/GameObjectLuaBinder.h"
#include "gameobject/GameObject.h"
#include "input/TouchInputProcessor.h"
#include "render/GlobalLight.h"
#include "render/LocalLight.h"
#include <bt-assimp/include/aiScene.h>
#include <batterytech/render/GLResourceManager.h>
#include <batterytech/render/GLAssimpBinding.h>
#include "render/WorldRenderer.h"

#define LUA_GAME "Game"
#define LUA_GAME_MT "GameMetaTable"

static GameContext *static_context;

// c metafunctions available to lua
static int lua_Game_gc (lua_State *L);
static int lua_Game_tostring (lua_State *L);

// c functions available to lua
static int lua_Game_getInstance(lua_State *L);
static int lua_Game_replaceMetatable(lua_State *L);
static int lua_Game_addScreenControl(lua_State *L);
static int lua_Game_updateScreenControl(lua_State *L);
static int lua_Game_updateScreenControlTexture(lua_State *L);
static int lua_Game_removeScreenControl(lua_State *L);
static int lua_Game_clearScreenControls(lua_State *L);
static int lua_Game_loadLevelFromFile(lua_State *L);
static int lua_Game_quit(lua_State *L);
static int lua_Game_renderStaticObjM(lua_State *L);
static int lua_Game_renderText2D(lua_State *L);
static int lua_Game_render2D(lua_State *L);
static int lua_Game_render2DBG(lua_State *L);
static int lua_Game_renderDynamicM(lua_State *L);
static int lua_Game_renderBB(lua_State *L);
static int lua_Game_setShadowLightOrigin(lua_State *L);
static int lua_Game_setShadowColorAndEpsilon(lua_State *L);
static int lua_Game_setShadowLightFrustumNearFar(lua_State *L);
static int lua_Game_setGlobalLightDir(lua_State *L);
static int lua_Game_setGlobalLightAmbient(lua_State *L);
static int lua_Game_setGlobalLightDiffuse(lua_State *L);
static int lua_Game_setGlobalLightSpecular(lua_State *L);
static int lua_Game_setRenderItemParam(lua_State *L);
static int lua_Game_loadNewResources(lua_State *L);
static int lua_Game_addLocalLight(lua_State *L);
static int lua_Game_setLocalLightParam(lua_State *L);
static int lua_Game_clearLocalLights(lua_State *L);
static int lua_Game_addLocalLightsFromAssimp(lua_State *L);
static int lua_Game_measureText(lua_State *L);

static const luaL_reg lua_methods[] = {
	{ "getInstance", lua_Game_getInstance },
	{ "replaceMetatable", lua_Game_replaceMetatable },
	{ "addScreenControl", lua_Game_addScreenControl },
	{ "updateScreenControl", lua_Game_updateScreenControl },
	{ "updateScreenControlTexture", lua_Game_updateScreenControlTexture },
	{ "removeScreenControl", lua_Game_removeScreenControl },
	{ "clearScreenControls", lua_Game_clearScreenControls },
	{ "loadLevelFromFile", lua_Game_loadLevelFromFile },
	{ "quit", lua_Game_quit },
	{ "renderStaticObjM", lua_Game_renderStaticObjM },
	{ "renderDynamicM", lua_Game_renderDynamicM },
	{ "renderText2D", lua_Game_renderText2D },
	{ "render2D", lua_Game_render2D },
	{ "render2DBG", lua_Game_render2DBG },
	{ "renderBB", lua_Game_renderBB },
	{ "setRenderItemParam", lua_Game_setRenderItemParam },
	{ "loadNewResources", lua_Game_loadNewResources },
	{ "setShadowLightOrigin", lua_Game_setShadowLightOrigin },
	{ "setShadowColorAndEpsilon", lua_Game_setShadowColorAndEpsilon },
	{ "setShadowLightFrustumNearFar", lua_Game_setShadowLightFrustumNearFar },
	{ "setGlobalLightDir", lua_Game_setGlobalLightDir },
	{ "setGlobalLightAmbient", lua_Game_setGlobalLightAmbient },
	{ "setGlobalLightDiffuse", lua_Game_setGlobalLightDiffuse },
	{ "setGlobalLightSpecular", lua_Game_setGlobalLightSpecular },
	{ "addLocalLight", lua_Game_addLocalLight },
	{ "setLocalLightParam", lua_Game_setLocalLightParam },
	{ "clearLocalLights", lua_Game_clearLocalLights },
	{ "addLocalLightsFromAssimp", lua_Game_addLocalLightsFromAssimp },
	{ "measureText", lua_Game_measureText },
	{ 0, 0 } };

static const luaL_reg metatable[] = {
	{"__gc",       lua_Game_gc},
	{"__tostring", lua_Game_tostring},
	{0, 0}
};

GameLuaBinder::GameLuaBinder(lua_State *L, Game *game, GameContext *context) :
	LuaBinder(L) {
	this->game = game;
	this->context = context;
	gameRef = 0;
}

GameLuaBinder::~GameLuaBinder() {
	game = NULL;
	static_context = NULL;
}

void GameLuaBinder::luaRegister(lua_State *L, GameContext *context) {
	static_context = context;
	luaL_openlib(L, LUA_GAME, lua_methods, 0);  /* create methods table, add it to the globals */
	luaL_newmetatable(L, LUA_GAME_MT);        /* create metatable for Game,  add it to the Lua registry */
	luaL_openlib(L, 0, metatable, 0);  /* fill metatable */
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);               /* dup methods table*/
	lua_rawset(L, -3);                  /* metatable.__index = Game methods */
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, -2);               /* dup methods table*/
	lua_rawset(L, -3);                  /* hide metatable:  metatable.__metatable = metatable methods */
	lua_settop(L, -3); // put the stack back to where we started
}

char* GameLuaBinder::getString(const char* name) {
	char *string;
	lua_rawgeti( L, LUA_REGISTRYINDEX, gameRef ); // push the gameRef we made onto the stack - userdata at -1
	lua_pushstring( L, name ); // update, metatable, userdata
	lua_gettable(L, -2); // get the function from the __index (don't use rawget since it doesn't use index)
	if (lua_isstring(L, -1)) {
		string = strDuplicate(lua_tostring(L, -1));
	} else {
		string = NULL;
	}
	lua_settop(L, -3); // put the stack back to where we started
	return string;
}

BOOL32 GameLuaBinder::getBool(const char* name) {
	BOOL32 result = FALSE;
	lua_rawgeti( L, LUA_REGISTRYINDEX, gameRef ); // push the gameRef we made onto the stack - userdata at -1
	lua_pushstring( L, name ); // update, metatable, userdata
	lua_gettable(L, -2); // get the function from the __index (don't use rawget since it doesn't use index)
	if (lua_isboolean(L, -1)) {
		result = lua_toboolean(L, -1);
	}
	lua_settop(L, -3); // put the stack back to where we started
	return result;
}

S32 GameLuaBinder::getInt(const char* name) {
	S32 result = 0;
	lua_rawgeti( L, LUA_REGISTRYINDEX, gameRef ); // push the gameRef we made onto the stack - userdata at -1
	lua_pushstring( L, name ); // update, metatable, userdata
	lua_gettable(L, -2); // get the function from the __index (don't use rawget since it doesn't use index)
	if (lua_isnumber(L, -1)) {
		result = lua_tointeger(L, -1);
	}
	lua_settop(L, -3); // put the stack back to where we started
	return result;
}

F32 GameLuaBinder::getFloat(const char* name) {
	F32 result = 0;
	lua_rawgeti( L, LUA_REGISTRYINDEX, gameRef ); // push the gameRef we made onto the stack - userdata at -1
	lua_pushstring( L, name ); // update, metatable, userdata
	lua_gettable(L, -2); // get the function from the __index (don't use rawget since it doesn't use index)
	if (lua_isnumber(L, -1)) {
		result = (F32)lua_tonumber(L, -1);
	}
	lua_settop(L, -3); // put the stack back to where we started
	return result;
}

GameEditorObject* GameLuaBinder::getEditorObjects(S32 *count) {
	lua_rawgeti( L, LUA_REGISTRYINDEX, gameRef ); // push the gameRef we made onto the stack - userdata at -1
	lua_pushstring( L, "editorObjects" ); // update, metatable, userdata
	lua_gettable(L, -2); // get the editor objects table from the __index (don't use rawget since it doesn't use index)
	char buf[255];
	GameEditorObject *editorObjs = NULL;
	if (lua_istable(L, -1)) {
		logmsg("EditorObjects table found");
		*count = 0;
		for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
			(*count)++;
		}
		sprintf(buf, "%d EditorObjects Counted", *count);
		logmsg(buf);
		editorObjs = new GameEditorObject[*count];
		S32 i = 0;
		for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
			if (lua_isstring(L, -2) && lua_isstring(L, -1)) {
				char *className = strDuplicate(lua_tostring(L, -2));
				char *name = strDuplicate(lua_tostring(L, -1));
				sprintf(buf, "EditorObject %s=%s", className, name);
				logmsg(buf);
				editorObjs[i].className = className;
				editorObjs[i].name = name;
				i++;
			}
		}
	} else {
		// no objects
	}
	lua_settop(L, -3); // put the stack back to where we started
	return editorObjs;
}

void GameLuaBinder::editorAddObject(const char *className) {
	if (pushInstanceFunction("editorAddObject")) {
		callFunctionVA("Game:editorAddObject", TRUE, "s>", className);
	}
}

void GameLuaBinder::editorCopyObject(GameObject *obj) {
	if (pushInstanceFunction("editorCopyObject")) {
		callFunctionVA("Game:editorCopyObject", TRUE, "r>", obj->luaBinder->luaRef);
	}
}
void GameLuaBinder::editorSave() {
	if (pushInstanceFunction("editorSave")) {
		callFunctionVA("Game:editorSave", TRUE, ">");
	}
}

void GameLuaBinder::editorEnable(BOOL32 enabled) {
	if (pushInstanceFunction("editorEnable")) {
		callFunctionVA("Game:editorEnable", TRUE, "b>", enabled);
	}
}

void GameLuaBinder::editorFreelook(BOOL32 enabled) {
	if (pushInstanceFunction("editorFreelook")) {
		callFunctionVA("Game:editorFreelook", TRUE, "b>", enabled);
	}
}
void GameLuaBinder::editorDelete(GameObject *obj) {
	if (pushInstanceFunction("editorDelete")) {
		callFunctionVA("Game:editorDelete", TRUE, "r>", obj->luaBinder->luaRef);
	}
}

void GameLuaBinder::editorUseCurrentTransform(GameObject *obj) {
	if (pushInstanceFunction("editorUseCurrentTransform")) {
		callFunctionVA("Game:editorUseCurrentTransform", TRUE, "r>", obj->luaBinder->luaRef);
	}
}

void GameLuaBinder::init() {
	if (pushInstanceFunction("init")) {
		callFunction("Game:init", TRUE);
	}
}

void GameLuaBinder::update() {
	// clear the stack.
	if (lua_gettop(L) > 0) {
		char buf[255];
		sprintf(buf, "lua top is at %d on GameLuaBinder::update() - check for lua-c stack leaks", lua_gettop(L));
		logmsg(buf);
		stackDump(L);
		lua_settop(L, 0);
	}
	if (pushInstanceFunction("update")) {
		callFunctionVA("Game:update", TRUE, "d>", context->tickDelta);
	}
	// I guess lua handles this automatically?
	lua_gc(L, LUA_GCCOLLECT, 0);
}

void GameLuaBinder::render(S32 state) {
	if (pushInstanceFunction("render")) {
		callFunctionVA("Game:render", TRUE, "i>", state);
	}
}

void GameLuaBinder::reset(BOOL32 invalidate) {
	if (pushInstanceFunction("reset")) {
		callFunctionVA("Game:reset", TRUE, "i>", invalidate);
	}
}

void GameLuaBinder::setMode(S32 mode) {
	if (pushInstanceFunction("setMode")) {
		callFunctionVA("Game:setMode", TRUE, "i>", mode);
	}
}

void GameLuaBinder::screenControlPointerDown(const char *name, F32 x, F32 y) {
	if (pushInstanceFunction("screenControlPointerDown")) {
		callFunctionVA("Game:screenControlPointerDown", TRUE, "sdd>", name, x, y);
	}
}

void GameLuaBinder::screenControlPointerMove(const char *name, F32 x, F32 y) {
	if (pushInstanceFunction("screenControlPointerMove")) {
		callFunctionVA("Game:screenControlPointerMove", TRUE, "sdd>", name, x, y);
	}
}

void GameLuaBinder::screenControlPointerUp(const char *name, F32 x, F32 y) {
	if (pushInstanceFunction("screenControlPointerUp")) {
		callFunctionVA("Game:screenControlPointerUp", TRUE, "sdd>", name, x, y);
	}
}

BOOL32 GameLuaBinder::pushInstanceFunction(const char *name) {
	lua_rawgeti( L, LUA_REGISTRYINDEX, gameRef ); // push the gameRef we made onto the stack - userdata at -1
	lua_pushstring( L, name ); // update, metatable, userdata
	lua_gettable(L, -2); // get the function from the __index (don't use rawget since it doesn't use index)
	lua_remove( L, -2 ); //we have the function now so no longer need the name of it.  // function, table, userdata
	if( lua_isfunction( L, -1 ) == 1 ) {
		lua_rawgeti( L, LUA_REGISTRYINDEX, gameRef ); // use "self" as parameter
		return TRUE;
	}
	lua_settop(L, -2);
	return FALSE;
}

static int lua_Game_getInstance(lua_State *L) {
	// We use a double pointer because we already have an instance of Game we want to use.
	Game **lGamePtr = (Game **) lua_newuserdata(L, sizeof(Game*));
	*lGamePtr = static_context->game; // assign the pointer to our already-established game object
	lua_pushvalue( L, -1 ); // push game userdata on the stack
	(*lGamePtr)->luaBinder->gameRef = luaL_ref( L, LUA_REGISTRYINDEX ); // make a ref number for the lua object
	luaL_getmetatable(L, LUA_GAME_MT); // get the metatable we made which is meant for the game instance
	lua_setmetatable(L, -2); // set the metatable to the game instance
	return 1;
}

// Game:addScreenControl(name, label, textureAssetName, u1,v1,u2,v2, x1,y1,x2,y2, x3,y3,x4,y4, isInteractive)
static int lua_Game_addScreenControl(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	const char *name = lua_tostring(L, 2);
	const char *label = lua_tostring(L, 3);
	ScreenControl *sc = new ScreenControl(name, label);
	if (lua_isstring(L, 4)) {
		sc->setTextureAssetName(lua_tostring(L, 4));
	} else {
		sc->setTextureAssetName(NULL);
	}
	sc->textureUVs = lua_toVector4f(L, 5);
	sc->drawableBounds = lua_toVector4f(L, 9);
	sc->pointerBounds = lua_toVector4f(L, 13);
	sc->isInteractive = lua_toboolean(L, 17);
	static_context->world->screenControls->add(sc);
	return 0;
}

// Game:updateScreenControl(name, label, textureAssetName, u1,v1,u2,v2, x1,y1,x2,y2, x3,y3,x4,y4, isInteractive)
static int lua_Game_updateScreenControl(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	const char *name = lua_tostring(L, 2);
	const char *label = lua_tostring(L, 3);
	ManagedArray<ScreenControl> *screenControls = static_context->world->screenControls;
	for (S32 i = 0; i < screenControls->getSize(); i++) {
		if (strEquals(name, screenControls->array[i]->getName())) {
			ScreenControl *sc = screenControls->array[i];
			sc->setLabel(label);
			sc->setTextureAssetName(lua_tostring(L, 4));
			sc->textureUVs = lua_toVector4f(L, 5);
			sc->drawableBounds = lua_toVector4f(L, 9);
			sc->pointerBounds = lua_toVector4f(L, 13);
			sc->isInteractive = lua_toboolean(L, 17);
		}
	}
	return 0;
}

// Game:updateScreenControlTexture(name, textureAssetName, u1,v1,u2,v2)
static int lua_Game_updateScreenControlTexture(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	const char *name = lua_tostring(L, 2);
	ManagedArray<ScreenControl> *screenControls = static_context->world->screenControls;
	for (S32 i = 0; i < screenControls->getSize(); i++) {
		if (strEquals(name, screenControls->array[i]->getName())) {
			ScreenControl *sc = screenControls->array[i];
			sc->setTextureAssetName(lua_tostring(L, 3));
			sc->textureUVs = lua_toVector4f(L, 4);
		}
	}
	return 0;
}

// Game:removeScreenControl(name)
static int lua_Game_removeScreenControl(lua_State *L) {
	Game *game = *(Game**)lua_touserdata(L, 1);
	const char *name = lua_tostring(L, 2);
	ManagedArray<ScreenControl> *screenControls = static_context->world->screenControls;
	for (S32 i = 0; i < screenControls->getSize(); i++) {
		if (strEquals(name, screenControls->array[i]->getName())) {
			ScreenControl *sc = screenControls->array[i];
			screenControls->remove(i);
			game->touchInputProcessor->notifyScreenControlRemoved(sc);
			delete sc;
		}
	}
	return 0;
}

// Game:clearScreenControls()
static int lua_Game_clearScreenControls(lua_State *L) {
	Game *game = *(Game**)lua_touserdata(L, 1);
	ManagedArray<ScreenControl> *screenControls = static_context->world->screenControls;
	for (S32 i = 0; i < screenControls->getSize(); i++) {
		game->touchInputProcessor->notifyScreenControlRemoved(screenControls->array[i]);
	}
	static_context->world->screenControls->deleteElements();
	return 0;
}

static int lua_Game_loadLevelFromFile(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	const char *assetName = lua_tostring(L, 2);
	static_context->world->level->load(assetName);
	return 0;
}

static int lua_Game_loadNewResources(lua_State *L) {
	static_context->world->gameState = GAMESTATE_LOADING;
	static_context->world->nextGameState = GAMESTATE_RUNNING;
	return 0;
}

static int lua_Game_quit(lua_State *L) {
	static_context->uiManager->clearMenuStack();
	static_context->world->gameState = GAMESTATE_READY;
	return 0;
}

// objName, groupName, textureAssetname, isOpaque, Column-Major World Transform 4x4, model scale
static int lua_Game_renderStaticObjM(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	if (static_context->world->renderItemsUsed == MAX_RENDERITEMS) {
		lua_pushinteger(L, -1);
		return 1;
	}
	RenderItem *item = &static_context->world->renderItems[static_context->world->renderItemsUsed++];
	item->reset();
	item->renderType = RenderItem::RENDERTYPE_OBJ;
	const char *objName = lua_tostring(L, 2);
	strcpy(item->resourceName, objName);
	if (lua_isstring(L, 3)) {
		const char *groupName = lua_tostring(L, 3); // if nil, render whole scene
		strcpy(item->attr1, groupName);
	} else {
		item->attr1[0] = '\0';
	}
	if (lua_isstring(L, 4)) {
		const char *textureAssetName = lua_tostring(L, 4); // if nil, use obj materials
		strcpy(item->textureName, textureAssetName);
	} else {
		item->textureName[0] = '\0';
	}
	BOOL32 isOpaque = lua_toboolean(L, 5);
	if (isOpaque) {
		item->flags = item->flags | RENDERITEM_FLAG_IS_OPAQUE;
	}
	Matrix4f mat = lua_toMatrix4f(L, 6);
	item->flags = item->flags | RENDERITEM_FLAG_HAS_MATRIX;
	item->mat = mat;
	if (lua_isnumber(L, 22) && lua_isnumber(L, 23) && lua_isnumber(L, 24)) {
		Vector3f scale = lua_toVector3f(L, 22);
		item->mat.scale(scale.x,scale.y,scale.z);
	}
	lua_pushinteger(L, static_context->world->renderItemsUsed-1);
	return 1;
}

// GameObject, animatorIdx, objName, groupName, textureAssetname, isOpaque, Column-Major World Transform 4x4, model scale
static int lua_Game_renderDynamicM(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	if (static_context->world->renderItemsUsed == MAX_RENDERITEMS) {
		lua_pushinteger(L, -1);
		return 1;
	}
	RenderItem *item = &static_context->world->renderItems[static_context->world->renderItemsUsed++];
	item->reset();
	item->renderType = RenderItem::RENDERTYPE_ASSIMP;
	if (lua_isuserdata(L, 2)) {
		GameObject *obj = *(GameObject**)lua_touserdata(L, 2);
		S32 animatorIdx = lua_tointeger(L, 3);
		item->animator = obj->animators->array[animatorIdx];
	} else {
		item->animator = NULL;
	}
	const char *objName = lua_tostring(L, 4);
	strcpy(item->resourceName, objName);
	if (lua_isstring(L, 5)) {
		const char *groupName = lua_tostring(L, 5); // if nil, render whole scene
		strcpy(item->attr1, groupName);
	} else {
		item->attr1[0] = '\0';
	}
	if (lua_isstring(L, 6)) {
		const char *textureAssetName = lua_tostring(L, 6); // if nil, use obj materials
		strcpy(item->textureName, textureAssetName);
	} else {
		item->textureName[0] = '\0';
	}
	BOOL32 isOpaque = lua_toboolean(L, 7);
	if (isOpaque) {
		item->flags = item->flags | RENDERITEM_FLAG_IS_OPAQUE;
	}
	Matrix4f mat = lua_toMatrix4f(L, 8);
	item->flags = item->flags | RENDERITEM_FLAG_HAS_MATRIX;
	item->mat = mat;
	if (lua_isnumber(L, 24) && lua_isnumber(L, 25) && lua_isnumber(L, 26)) {
		Vector3f scale = lua_toVector3f(L, 24);
		item->mat.scale(scale.x,scale.y,scale.z);
	}
	if (lua_isnumber(L, 27)) {
		// Z-axis rotation correction
		item->mat.rotate(lua_tonumber(L, 27), 0, 0, 1);
	}
	lua_pushinteger(L, static_context->world->renderItemsUsed-1);
	return 1;
}

// (tag), text, position2
static int lua_Game_renderText2D(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	if (static_context->world->renderItemsUsed == MAX_RENDERITEMS) {
		lua_pushinteger(L, -1);
		return 1;
	}
	RenderItem *item = &static_context->world->renderItems[static_context->world->renderItemsUsed++];
	item->reset();
	item->renderType = RenderItem::RENDERTYPE_TEXT2D;
	const char *tag = FONT_TAG_UI;
	const char* text;
	F32 x, y;
	if (lua_gettop(L) == 5) {
		// has tag
		tag = lua_tostring(L, 2);
		text = lua_tostring(L, 3);
		x = lua_tonumber(L, 4);
		y = lua_tonumber(L, 5);
	} else {
		text = lua_tostring(L, 2);
		x = lua_tonumber(L, 3);
		y = lua_tonumber(L, 4);
	}
	strcpy(item->attr1, text);
	strcpy(item->attr2, tag);
	item->pos = Vector3f(x,y,0);
	lua_pushinteger(L, static_context->world->renderItemsUsed-1);
	return 1;
}

// asset, position2, scale2, rotation1
static int lua_Game_render2D(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	if (static_context->world->renderItemsUsed == MAX_RENDERITEMS) {
		lua_pushinteger(L, -1);
		return 1;
	}
	RenderItem *item = &static_context->world->renderItems[static_context->world->renderItemsUsed++];
	item->reset();
	item->renderType = RenderItem::RENDERTYPE_2D;
	const char *assetName = lua_tostring(L, 2);
	F32 x = lua_tonumber(L, 3);
	F32 y = lua_tonumber(L, 4);
	F32 width = lua_tonumber(L, 5);
	F32 height = lua_tonumber(L, 6);
	F32 zRotation = lua_tonumber(L, 7);
	F32 alpha = 1.0;
	if(lua_isnumber(L, 8)) {
		alpha = lua_tonumber(L, 8);
	}
	strcpy(item->textureName, assetName);
	item->pos = Vector3f(x,y,0);
	item->scale = Vector3f(width, height, 0);
	item->orientation.v = Vector3f(0,0,zRotation);
	if(alpha < 1.0) {
		item->flags = item->flags & ~RENDERITEM_FLAG_IS_OPAQUE;
	} else {
		item->flags = item->flags | RENDERITEM_FLAG_IS_OPAQUE;
	}
	item->alpha = alpha;
	lua_pushinteger(L, static_context->world->renderItemsUsed-1);
	return 1;
}

// asset, position2, scale2, rotation1
static int lua_Game_render2DBG(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	if (static_context->world->renderItemsUsed == MAX_RENDERITEMS) {
		lua_pushboolean(L, FALSE);
		return 1;
	}
	RenderItem *item = &static_context->world->renderItems[static_context->world->renderItemsUsed++];
	item->reset();
	item->renderType = RenderItem::RENDERTYPE_2DBG;
	const char *assetName = lua_tostring(L, 2);
	F32 x = lua_tonumber(L, 3);
	F32 y = lua_tonumber(L, 4);
	F32 width = lua_tonumber(L, 5);
	F32 height = lua_tonumber(L, 6);
	F32 zRotation = lua_tonumber(L, 7);
	F32 alpha = 1.0;
	if(lua_isnumber(L, 8)) {
		alpha = lua_tonumber(L, 8);
	}
	strcpy(item->textureName, assetName);
	item->pos = Vector3f(x,y,0);
	item->scale = Vector3f(width, height, 0);
	item->orientation.v = Vector3f(0,0,zRotation);
	if(alpha < 1.0) {
		item->flags = item->flags & ~RENDERITEM_FLAG_IS_OPAQUE;
	} else {
		item->flags = item->flags | RENDERITEM_FLAG_IS_OPAQUE;
	}
	item->alpha = alpha;
	lua_pushinteger(L, static_context->world->renderItemsUsed-1);
	return 1;
}
// asset, position3, scale2, rotation1
static int lua_Game_renderBB(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	if (static_context->world->renderItemsUsed == MAX_RENDERITEMS) {
		lua_pushinteger(L, -1);
		return 1;
	}
	RenderItem *item = &static_context->world->renderItems[static_context->world->renderItemsUsed++];
	item->reset();
	item->renderType = RenderItem::RENDERTYPE_BB;
	const char *assetName = lua_tostring(L, 2);
	F32 x = lua_tonumber(L, 3);
	F32 y = lua_tonumber(L, 4);
	F32 z = lua_tonumber(L, 5);
	F32 width = lua_tonumber(L, 6);
	F32 height = lua_tonumber(L, 7);
	F32 zRotation = lua_tonumber(L, 8);
	F32 alpha = 1.0;
	if(lua_isnumber(L, 9)) {
		alpha = lua_tonumber(L, 9);
	}
	if (lua_isnumber(L, 10) && lua_isnumber(L, 11) && lua_isnumber(L, 12) && lua_isnumber(L, 13)) {
		item->uvs = lua_toVector4f(L, 10);
	} else {
		item->uvs = Vector4f(0,0,1,1);
	}
	strcpy(item->textureName, assetName);
	item->pos = Vector3f(x,y,z);
	item->scale = Vector3f(width, height, 0);
	item->orientation.v = Vector3f(0,0,zRotation);
	if(alpha < 1.0) {
		item->flags = item->flags & ~RENDERITEM_FLAG_IS_OPAQUE;
	} else {
		item->flags = item->flags | RENDERITEM_FLAG_IS_OPAQUE;
	}
	item->alpha = alpha;
	lua_pushinteger(L, static_context->world->renderItemsUsed-1);
	return 1;
}

static int lua_Game_setGlobalLightDir(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->globalLight->direction = lua_toVector3f(L, 2);
	static_context->world->globalLight->direction.normalize();
	return 0;
}

static int lua_Game_setGlobalLightAmbient(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->globalLight->ambient = lua_toVector4f(L, 2);
	return 0;
}

static int lua_Game_setGlobalLightDiffuse(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->globalLight->diffuse = lua_toVector4f(L, 2);
	return 0;
}

static int lua_Game_setGlobalLightSpecular(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->globalLight->specular = lua_toVector4f(L, 2);
	return 0;
}

static int lua_Game_setShadowLightOrigin(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->globalLight->origin = lua_toVector3f(L, 2);
	return 0;
}

static int lua_Game_setShadowColorAndEpsilon(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->globalLight->shadowColor = lua_toVector3f(L, 2);
	static_context->world->globalLight->shadowEpsilon = lua_tonumber(L, 5);
	return 0;
}

static int lua_Game_setShadowLightFrustumNearFar(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->globalLight->shadowFrustumNearFar = lua_toVector2f(L, 2);
	return 0;
}


// vec3 position
static int lua_Game_addLocalLight(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	if (static_context->world->localLightsUsed == MAX_LOCALLIGHTS) {
		lua_pushinteger(L, -1);
		return 1;
	}
	LocalLight *light = &static_context->world->localLights[static_context->world->localLightsUsed++];
	light->reset();
	Vector3f pos = lua_toVector3f(L, 2);
	light->position = pos;
	lua_pushinteger(L, static_context->world->localLightsUsed-1);
	return 1;
}

static int lua_Game_setLocalLightParam(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	S32 idx = lua_tointeger(L, 2);
	LocalLight *light = &static_context->world->localLights[idx];
	const char *paramName = lua_tostring(L, 3);
	if (strcmp(paramName, "ambient") == 0) {
		light->ambient = lua_toVector4f(L, 4);
	} else if (strcmp(paramName, "diffuse") == 0) {
		light->diffuse = lua_toVector4f(L, 4);
	} else if (strcmp(paramName, "specular") == 0) {
		light->specular = lua_toVector4f(L, 4);
	} else if (strcmp(paramName, "attenuation") == 0) {
		light->constantAttenuation = lua_tonumber(L, 4);
		light->linearAttenuation = lua_tonumber(L, 5);
		light->quadraticAttenuation = lua_tonumber(L, 6);
	}
	return 0;
}

static int lua_Game_clearLocalLights(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->localLightsUsed = 0;
	return 0;
}

static int addLocalLightsForNode(const aiScene *scene, aiNode *node, const char *prefix, aiMatrix4x4 transform) {
	if (static_context->world->localLightsUsed == MAX_LOCALLIGHTS) {
		return 0;
	}
	S32 lightsAdded = 0;
	if (prefix[0] == '\0' || strStartsWith(node->mName.data, prefix)) {
		for (U32 i = 0; i < node->mNumMeshes; i++) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			// find centerpoint of mesh
			aiVector3D centerPoint;
			for (U32 j = 0; j < mesh->mNumVertices; j++) {
				aiVector3D vert = mesh->mVertices[j];
				centerPoint += vert;
			}
			centerPoint /= mesh->mNumVertices;
			centerPoint *= transform;
			LocalLight *light = &static_context->world->localLights[static_context->world->localLightsUsed++];
			light->reset();
			Vector3f pos = Vector3f(centerPoint.x, centerPoint.y, centerPoint.z);
			light->position = pos;
			lightsAdded++;
		}
	}
	for (U32 i = 0; i < node->mNumChildren; i++) {
		lightsAdded += addLocalLightsForNode(scene, node->mChildren[i], prefix, transform * node->mTransformation);
	}
	return lightsAdded;
}

static int lua_Game_addLocalLightsFromAssimp(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	const char *assetName = lua_tostring(L, 2);
	const char *meshPrefix = NULL;
	if (lua_isstring(L, 3)) {
		meshPrefix = lua_tostring(L, 3);
	}
	S32 lastUsedIdx = static_context->world->localLightsUsed - 1;
	S32 lightsAdded = 0;
	//find asset
	GLAssimpBinding *assimpBinding = static_context->glResourceManager->getAssimp(assetName);
	if (assimpBinding) {
		aiMatrix4x4 transform;
		lightsAdded = addLocalLightsForNode(assimpBinding->scene, assimpBinding->scene->mRootNode, meshPrefix, transform);
	}
	S32 firstIdx = lastUsedIdx + (lightsAdded? 1 : 0);
	lua_pushinteger(L, firstIdx);
	lua_pushinteger(L, firstIdx + lightsAdded-1);
	return 2;
}

// game, renderItemIdx, paramName, values...
static int lua_Game_setRenderItemParam(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	S32 idx = lua_tointeger(L, 2);
	RenderItem *item = &static_context->world->renderItems[idx];
	const char *paramName = lua_tostring(L, 3);
	if (strcmp(paramName, "nofog") == 0) {
		if (lua_toboolean(L, 4)) {
			item->flags = item->flags | RENDERITEM_FLAG_NO_FOG;
		} else {
			item->flags = item->flags & ~RENDERITEM_FLAG_NO_FOG;
		}
	} else if (strcmp(paramName, "maxPointLights") == 0) {
		item->maxPointLights = lua_tointeger(L, 4);
	} else if (strcmp(paramName, "useCameraClosestPointLights") == 0) {
		if (lua_toboolean(L, 4)) {
			item->flags = item->flags | RENDERITEM_FLAG_USE_CAMERA_CLOSEST_POINT_LIGHTS;
		} else {
			item->flags = item->flags & ~RENDERITEM_FLAG_USE_CAMERA_CLOSEST_POINT_LIGHTS;
		}
	} else if (strcmp(paramName, "viewport") == 0) {
		item->viewport = lua_toVector4i(L, 4);
	} else if (strcmp(paramName, "uvs") == 0) {
		item->uvs = lua_toVector4f(L, 4);
	} else if (strcmp(paramName, "align") == 0) {
		const char *paramValue = lua_tostring(L, 4);
		if (strcmp(paramValue, "left") == 0) {
			item->alignment = RenderItem::ALIGN_LEFT;
		} else if (strcmp(paramValue, "center") == 0) {
			item->alignment = RenderItem::ALIGN_CENTER;
		} else if (strcmp(paramValue, "right") == 0) {
			item->alignment = RenderItem::ALIGN_RIGHT;
		}
	} else if (strcmp(paramName, "nodesCullFrustum") == 0) {
		if (lua_toboolean(L, 4)) {
			item->flags = item->flags | RENDERITEM_FLAG_NODES_CULL_FRUSTUM_TEST;
		} else {
			item->flags = item->flags & ~RENDERITEM_FLAG_NODES_CULL_FRUSTUM_TEST;
		}
	}
	return 0;
}

// game, tag, text -- returns width/height
static int lua_Game_measureText(lua_State *L) {
	Game *game = *(Game**)lua_touserdata(L, 1);
	const char *tag = lua_tostring(L, 2);
	const char *text = lua_tostring(L, 3);
	F32 scale = 1.0f;
	if (lua_isnumber(L, 4)) {
		scale = lua_tonumber(L, 4);
	}
	TextRasterRenderer *renderer = static_context->worldRenderer->getTextRenderer(tag);
	if (renderer) {
		lua_pushnumber(L, renderer->measureWidth(text, scale));
		lua_pushnumber(L, renderer->getHeight(scale));
	} else {
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);
	}
	return 2;
}

// --------------- metamethods ----------------

static int lua_Game_replaceMetatable(lua_State *L) {
	lua_setmetatable(L, -2); // -1 should be self, -2 should be metatable
	return 0;
}

static int lua_Game_gc (lua_State *L) {
  printf("goodbye Game (%p)\n", *(Game**)lua_touserdata(L, 1));
  return 0;
}

static int lua_Game_tostring (lua_State *L) {
  lua_pushfstring(L, "Game: %p", *(Game**)lua_touserdata(L, 1));
  return 1;
}
