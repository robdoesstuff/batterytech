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
#include <batterytech/render/RenderContext.h>
#include "gameobject/ParticleEmitter.h"

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
static int lua_Game_createPhysicsWorld(lua_State *L);
static int lua_Game_clearPhysicsWorld(lua_State *L);
static int lua_Game_destroyPhysicsWorld(lua_State *L);
static int lua_Game_updatePhysics(lua_State *L);
static int lua_Game_setPhysicsGravity(lua_State *L);
static int lua_Game_setPhysicsDrawDebug(lua_State *L);
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
static int lua_Game_renderAssimp(lua_State *L);
static int lua_Game_renderAssimpM(lua_State *L);
static int lua_Game_renderBB(lua_State *L);
static int lua_Game_setShadowLightOrigin(lua_State *L);
static int lua_Game_setShadowColorAndEpsilon(lua_State *L);
static int lua_Game_setShadowLightFrustumNearFar(lua_State *L);
static int lua_Game_setShadowOrtho(lua_State *L);
static int lua_Game_setShadowPerspective(lua_State *L);
static int lua_Game_getShadowType(lua_State *L);
static int lua_Game_setShadowType(lua_State *L);
static int lua_Game_setGlobalLightEnabled(lua_State *L);
static int lua_Game_setGlobalLightDir(lua_State *L);
static int lua_Game_setGlobalLightAmbient(lua_State *L);
static int lua_Game_setGlobalLightDiffuse(lua_State *L);
static int lua_Game_setGlobalLightSpecular(lua_State *L);
static int lua_Game_setFogEnabled(lua_State *L);
static int lua_Game_setFogParams(lua_State *L);
static int lua_Game_setRenderItemParam(lua_State *L);
static int lua_Game_loadNewResources(lua_State *L);
static int lua_Game_addLocalLight(lua_State *L);
static int lua_Game_setLocalLightParam(lua_State *L);
static int lua_Game_clearLocalLights(lua_State *L);
static int lua_Game_addLocalLightsFromAssimp(lua_State *L);
static int lua_Game_getMeshInfoFromAssimp(lua_State *L);
static int lua_Game_measureText(lua_State *L);
static int lua_Game_engineReset(lua_State *L);
static int lua_Game_addParticleEmitter(lua_State *L);
static int lua_Game_setParticleEmitterTimeRange(lua_State *L);
static int lua_Game_setParticleEmitterPosition(lua_State *L);
static int lua_Game_setParticleEmitterDirection(lua_State *L);
static int lua_Game_setParticleEmitterTextureAsset(lua_State *L);
static int lua_Game_removeParticleEmitter(lua_State *L);
static int lua_Game_stopParticleEmitter(lua_State *L);
static int lua_Game_clearParticleEmitters(lua_State *L);
static int lua_Game_startParticleEmitter(lua_State *L);
static int lua_Game_setParticleEmitterConeRange(lua_State *L);
static int lua_Game_setParticleScaleSpeedRange(lua_State *L);
static int lua_Game_setParticleAlphaSpeedRange(lua_State *L);
static int lua_Game_setParticleMaxSpeedRange(lua_State *L);
static int lua_Game_setParticleEmissionRate(lua_State *L);
static int lua_Game_setParticleInitialScale(lua_State *L);
static int lua_Game_setParticleRotationSpeedRange(lua_State *L);
static int lua_Game_setParticleGravity(lua_State *L);

static const luaL_reg lua_methods[] = {
	{ "getInstance", lua_Game_getInstance },
	{ "replaceMetatable", lua_Game_replaceMetatable },
	{ "createPhysicsWorld", lua_Game_createPhysicsWorld },
	{ "clearPhysicsWorld", lua_Game_clearPhysicsWorld },
	{ "destroyPhysicsWorld", lua_Game_destroyPhysicsWorld },
	{ "updatePhysics", lua_Game_updatePhysics },
	{ "setPhysicsGravity", lua_Game_setPhysicsGravity },
	{ "setPhysicsDrawDebug", lua_Game_setPhysicsDrawDebug },
	{ "addScreenControl", lua_Game_addScreenControl },
	{ "updateScreenControl", lua_Game_updateScreenControl },
	{ "updateScreenControlTexture", lua_Game_updateScreenControlTexture },
	{ "removeScreenControl", lua_Game_removeScreenControl },
	{ "clearScreenControls", lua_Game_clearScreenControls },
	{ "loadLevelFromFile", lua_Game_loadLevelFromFile },
	{ "quit", lua_Game_quit },
	{ "renderStaticObjM", lua_Game_renderStaticObjM },
	{ "renderAssimp", lua_Game_renderAssimp },
	{ "renderDynamic", lua_Game_renderAssimp }, // legacy
	{ "renderAssimpM", lua_Game_renderAssimpM },
	{ "renderDynamicM", lua_Game_renderAssimpM }, // legacy
	{ "renderText2D", lua_Game_renderText2D },
	{ "render2D", lua_Game_render2D },
	{ "render2DBG", lua_Game_render2DBG },
	{ "renderBB", lua_Game_renderBB },
	{ "setRenderItemParam", lua_Game_setRenderItemParam },
	{ "loadNewResources", lua_Game_loadNewResources },
	{ "getShadowType", lua_Game_getShadowType },
	{ "setShadowType", lua_Game_setShadowType },
	{ "setShadowLightOrigin", lua_Game_setShadowLightOrigin },
	{ "setShadowColorAndEpsilon", lua_Game_setShadowColorAndEpsilon },
	{ "setShadowLightFrustumNearFar", lua_Game_setShadowLightFrustumNearFar },
	{ "setShadowOrtho", lua_Game_setShadowOrtho },
	{ "setShadowPerspective", lua_Game_setShadowPerspective },
	{ "setGlobalLightEnabled", lua_Game_setGlobalLightEnabled },
	{ "setGlobalLightDir", lua_Game_setGlobalLightDir },
	{ "setGlobalLightAmbient", lua_Game_setGlobalLightAmbient },
	{ "setGlobalLightDiffuse", lua_Game_setGlobalLightDiffuse },
	{ "setGlobalLightSpecular", lua_Game_setGlobalLightSpecular },
	{ "setFogEnabled", lua_Game_setFogEnabled },
	{ "setFogParams", lua_Game_setFogParams },
	{ "addLocalLight", lua_Game_addLocalLight },
	{ "setLocalLightParam", lua_Game_setLocalLightParam },
	{ "clearLocalLights", lua_Game_clearLocalLights },
	{ "addLocalLightsFromAssimp", lua_Game_addLocalLightsFromAssimp },
	{ "getMeshInfoFromAssimp", lua_Game_getMeshInfoFromAssimp },
	{ "measureText", lua_Game_measureText },
	{ "engineReset", lua_Game_engineReset },
    { "addParticleEmitter", lua_Game_addParticleEmitter },
    { "setParticleEmitterTimeRange", lua_Game_setParticleEmitterTimeRange },
    { "setParticleEmitterPosition", lua_Game_setParticleEmitterPosition },
    { "setParticleEmitterDirection", lua_Game_setParticleEmitterDirection },
    { "setParticleEmitterTextureAsset", lua_Game_setParticleEmitterTextureAsset },
    { "removeParticleEmitter", lua_Game_removeParticleEmitter },
    { "stopParticleEmitter", lua_Game_stopParticleEmitter },
    { "clearParticleEmitters", lua_Game_clearParticleEmitters },
    { "startParticleEmitter", lua_Game_startParticleEmitter },
    { "setParticleEmitterConeRange", lua_Game_setParticleEmitterConeRange },
    { "setParticleScaleSpeedRange", lua_Game_setParticleScaleSpeedRange },
    { "setParticleInitialScale", lua_Game_setParticleInitialScale },
    { "setParticleAlphaSpeedRange", lua_Game_setParticleAlphaSpeedRange },
    { "setParticleMaxSpeedRange", lua_Game_setParticleMaxSpeedRange },
    { "setParticleEmissionRate", lua_Game_setParticleEmissionRate },
    { "setParticleRotationSpeedRange", lua_Game_setParticleRotationSpeedRange },
    { "setParticleGravity", lua_Game_setParticleGravity },
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

static int lua_Game_createPhysicsWorld(lua_State *L) {
    Game *game = *(Game**)lua_touserdata(L, 1);
#ifdef BATTERYTECH_INCLUDE_BOX2D
    b2Vec2 gravity(0.0f, DEFAULT_GRAVITY);
	static_context->world->boxWorld = new b2World(gravity, true);
	static_context->world->boxWorld->SetContinuousPhysics(game);
	static_context->world->boxWorld->SetContactListener(game);
	static_context->world->boxWorld->SetDestructionListener(game);
#endif
#ifdef BATTERYTECH_INCLUDE_BULLET
    // TODO - declare these in game and also clean them up when done
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_broadphase = new btDbvtBroadphase();
	sol = new btSequentialImpulseConstraintSolver;
    static_context->world->btWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,sol,m_collisionConfiguration);
#endif
    return 0;
}

static int lua_Game_clearPhysicsWorld(lua_State *L) {
    Game *game = *(Game**)lua_touserdata(L, 1);
#ifdef BATTERYTECH_INCLUDE_BOX2D
    if (static_context->world->boxWorld) {
    	while (static_context->world->boxWorld->GetBodyList()) {
    		static_context->world->boxWorld->DestroyBody(static_context->world->boxWorld->GetBodyList());
    	}
     }
#endif
#ifdef BATTERYTECH_INCLUDE_BULLET
#endif
    return 0;
}

static int lua_Game_destroyPhysicsWorld(lua_State *L) {
    Game *game = *(Game**)lua_touserdata(L, 1);
#ifdef BATTERYTECH_INCLUDE_BOX2D
    if (static_context->world->boxWorld) {
    	delete static_context->world->boxWorld;
    	static_context->world->boxWorld = NULL;
    }
#endif
#ifdef BATTERYTECH_INCLUDE_BULLET
#endif
    return 0;
}

static int lua_Game_updatePhysics(lua_State *L) {
    Game *game = *(Game**)lua_touserdata(L, 1);
    F32 delta = static_context->tickDelta;
    S32 substeps1 = 1;
    S32 substeps2 = 1;
    if (!lua_isnil(L, 2)) {
        delta = lua_tonumber(L, 2);
        if (!lua_isnil(L, 3)) {
            substeps1 = lua_tointeger(L, 3);
            if (!lua_isnil(L, 4)) {
                substeps2 = lua_tointeger(L, 4);
            }
        }
    }
    game->updatePhysics(delta, substeps1, substeps2);
    return 0;
}

static int lua_Game_setPhysicsGravity(lua_State *L) {
    // Game *game = *(Game**)lua_touserdata(L, 1);
    Vector3f gravity = lua_toVector3f(L, 2);
#ifdef BATTERYTECH_INCLUDE_BOX2D
    static_context->world->boxWorld->SetGravity(b2Vec2(gravity.x, gravity.y));
#endif
#ifdef BATTERYTECH_INCLUDE_BULLET
    static_context->world->btWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
#endif
    return 0;
}

static int lua_Game_setPhysicsDrawDebug(lua_State *L) {
	// Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->physicsDrawDebug = lua_toboolean(L, 2);
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

static int lua_Game_renderAssimp(lua_State* L) {
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
	item->flags = item->flags | RENDERITEM_FLAG_HAS_MATRIX;
    item->mat.identity();

    Vector3f translation;

	if (lua_isnumber(L, 8) && lua_isnumber(L, 9) && lua_isnumber(L, 10)) {
        translation = lua_toVector3f(L, 8);
    }
    item->mat.setTranslation(translation);

	if (lua_isnumber(L, 11) && lua_isnumber(L, 12) && lua_isnumber(L, 13)) {
		Vector3f scale = lua_toVector3f(L,11);
		item->mat.scale(scale.x,scale.y,scale.z);
	}

	if (lua_isnumber(L, 14) && lua_isnumber(L, 15) && lua_isnumber(L, 16)) {
		item->mat.rotate(lua_tonumber(L, 14), 1, 0, 0);
		item->mat.rotate(lua_tonumber(L, 15), 0, 1, 0);
		item->mat.rotate(lua_tonumber(L, 16), 0, 0, 1);
	}
	lua_pushinteger(L, static_context->world->renderItemsUsed-1);
	return 1;
}

// GameObject, animatorIdx, objName, groupName, textureAssetname, isOpaque, Column-Major World Transform 4x4, model scale
static int lua_Game_renderAssimpM(lua_State *L) {
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

// (tag), text, position2, (scale)
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
	F32 scale = 1.0f;
	if (lua_gettop(L) == 5) {
		// has tag
		tag = lua_tostring(L, 2);
		text = lua_tostring(L, 3);
		x = lua_tonumber(L, 4);
		y = lua_tonumber(L, 5);
		if (lua_isnumber(L, 6)) {
			scale = lua_tonumber(L, 6);
		}
	} else {
		text = lua_tostring(L, 2);
		x = lua_tonumber(L, 3);
		y = lua_tonumber(L, 4);
		if (lua_isnumber(L, 5)) {
			scale = lua_tonumber(L, 5);
		}
	}
	strcpy(item->attr1, text);
	strcpy(item->attr2, tag);
	item->pos = Vector3f(x,y,0);
	item->scale = Vector3f(scale, scale, 0);
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
	item->colorFilter.a = alpha;
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
	item->colorFilter.a = alpha;
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
	item->colorFilter.a = alpha;
	lua_pushinteger(L, static_context->world->renderItemsUsed-1);
	return 1;
}

static int lua_Game_setGlobalLightEnabled(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	((RenderDefaults*)static_context->renderContext->userValues->get("renderDefaults"))->dirLightEnabled = lua_toboolean(L, 2);
	return 0;
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

static int lua_Game_setFogEnabled(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	((RenderDefaults*)static_context->renderContext->userValues->get("renderDefaults"))->fogEnabled = lua_toboolean(L, 2);
	return 0;
}

static int lua_Game_setFogParams(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
    RenderDefaults *rDefaults = ((RenderDefaults*)static_context->renderContext->userValues->get("renderDefaults"));
	rDefaults->fogNear = lua_tonumber(L, 2);
	rDefaults->fogFar = lua_tonumber(L, 3);
    if (!lua_isnil(L, 4)) {
        rDefaults->fogColor = lua_toVector4f(L, 4);
    }
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

static int lua_Game_setShadowOrtho(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->globalLight->shadowUsePerspective = FALSE;
    static_context->world->globalLight->shadowOrthoSize = lua_toVector4f(L, 2);
	return 0;
}

static int lua_Game_setShadowPerspective(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->globalLight->shadowUsePerspective = TRUE;
    static_context->world->globalLight->shadowPerspectiveFOV = lua_tonumber(L, 2);
	return 0;
}

static int lua_Game_getShadowType(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	lua_pushinteger(L, static_context->gConfig->shadowType);
	return 1;
}

static int lua_Game_setShadowType(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	int sType = lua_tointeger(L, 2);
	if (sType == 0) {
		static_context->gConfig->shadowType = GraphicsConfiguration::SHADOWTYPE_NONE;
	} else if (sType == 1) {
		static_context->gConfig->shadowType = GraphicsConfiguration::SHADOWTYPE_SHADOWMAP;
	} else if (sType == 2) {
		static_context->gConfig->shadowType = GraphicsConfiguration::SHADOWTYPE_SHADOWMAP_HQ;
	}
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
	if (strcmp(paramName, "position") == 0) {
        light->position = lua_toVector3f(L, 4);
    } else if (strcmp(paramName, "ambient") == 0) {
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
	if (!prefix || prefix[0] == '\0' || strStartsWith(node->mName.data, prefix)) {
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

static void pushMeshTableOntoTable(lua_State *L, int tableIdx, int vecKey, Vector3f centerPoint, const char *name, U32 vertCount, Vector3f min, Vector3f max) {
	// push vector key (start with 1)
	lua_pushinteger(L, vecKey);
	// push vector value
	lua_createtable(L, 3, 0);
	int meshTableIndex = lua_gettop(L);
	// center x
	lua_pushinteger(L, 1);
	lua_pushnumber(L, centerPoint.x);
	lua_settable(L, meshTableIndex);
	// center y
	lua_pushinteger(L, 2);
	lua_pushnumber(L, centerPoint.y);
	lua_settable(L, meshTableIndex);
	// center z
	lua_pushinteger(L, 3);
	lua_pushnumber(L, centerPoint.z);
	lua_settable(L, meshTableIndex);
	// mesh name
	lua_pushinteger(L, 4);
	lua_pushstring(L, name);
	lua_settable(L, meshTableIndex);
	// vert count
	lua_pushinteger(L, 5);
	lua_pushinteger(L, vertCount);
	lua_settable(L, meshTableIndex);
	// min x
	lua_pushinteger(L, 6);
	lua_pushnumber(L, min.x);
	lua_settable(L, meshTableIndex);
	// min y
	lua_pushinteger(L, 7);
	lua_pushnumber(L, min.y);
	lua_settable(L, meshTableIndex);
	// min z
	lua_pushinteger(L, 8);
	lua_pushnumber(L, min.z);
	lua_settable(L, meshTableIndex);
	// max x
	lua_pushinteger(L, 9);
	lua_pushnumber(L, max.x);
	lua_settable(L, meshTableIndex);
	// max y
	lua_pushinteger(L, 10);
	lua_pushnumber(L, max.y);
	lua_settable(L, meshTableIndex);
	// max z
	lua_pushinteger(L, 11);
	lua_pushnumber(L, max.z);
	lua_settable(L, meshTableIndex);
	// now push mesh table key/value onto return table
	lua_settable(L, tableIdx);
}

static void pushMeshInfoForNode(lua_State *L, int tableIdx, int *vecNum, const aiScene *scene, aiNode *node, const char *prefix, aiMatrix4x4 transform) {
	if (!prefix || prefix[0] == '\0' || strStartsWith(node->mName.data, prefix)) {
		for (U32 i = 0; i < node->mNumMeshes; i++) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			// find centerpoint of mesh
			aiVector3D centerPoint;
			aiVector3D min(999999.0, 999999.0, 999999.0);
			aiVector3D max(-999999.0, -999999.0, -999999.0);
			for (U32 j = 0; j < mesh->mNumVertices; j++) {
				aiVector3D vert = mesh->mVertices[j];
				centerPoint += vert;
				if (vert.x < min.x) { min.x = vert.x; }
				if (vert.y < min.y) { min.y = vert.y; }
				if (vert.x < min.z) { min.z = vert.z; }
				if (vert.x > max.x) { max.x = vert.x; }
				if (vert.y > max.y) { max.y = vert.y; }
				if (vert.x > max.z) { max.z = vert.z; }
			}
			centerPoint /= mesh->mNumVertices;
			centerPoint *= transform;
			Vector3f pos(centerPoint.x, centerPoint.y, centerPoint.z);
			Vector3f vMin(min.x, min.y, min.z);
			Vector3f vMax(max.x, max.y, max.z);
			(*vecNum)++;
			pushMeshTableOntoTable(L, tableIdx, *vecNum, pos, node->mName.data, mesh->mNumVertices, vMin, vMax);
		}
	}
	for (U32 i = 0; i < node->mNumChildren; i++) {
		pushMeshInfoForNode(L, tableIdx, vecNum, scene, node->mChildren[i], prefix, transform * node->mTransformation);
	}
}

// gets mesh info for a mesh or multiple meshes in an assimp file
// returns table of tables(centerX, centerY, centerZ, name, vertCount, minX, minY, minZ, maxX, maxY, maxZ)
static int lua_Game_getMeshInfoFromAssimp(lua_State *L) {
	lua_newtable(L); // list of points
	int retTableIndex = lua_gettop(L);
	const char *assetName = lua_tostring(L, 2);
	const char *meshPrefix = NULL;
	if (lua_isstring(L, 3)) {
		meshPrefix = lua_tostring(L, 3);
	}
	GLAssimpBinding *assimpBinding = static_context->glResourceManager->getAssimp(assetName);
	if (assimpBinding) {
		aiMatrix4x4 transform;
		int vecNum = 0;
		pushMeshInfoForNode(L, retTableIndex, &vecNum, assimpBinding->scene, assimpBinding->scene->mRootNode, meshPrefix, transform);
	}
	// all we're returning is a single table of other mesh info tables
	return 1;
}

// game, renderItemIdx, paramName, values...
static int lua_Game_setRenderItemParam(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	S32 idx = lua_tointeger(L, 2);
	RenderItem *item = &static_context->world->renderItems[idx];
	const char *paramName = lua_tostring(L, 3);
    // TODO - optimize by hashing the names and selecting case
    if (strcmp(paramName, "uvs") == 0) {
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
	} else if (strcmp(paramName, "alpha") == 0) {
        item->colorFilter.a = lua_tonumber(L, 4);
    } else if (strcmp(paramName, "colorFilter") == 0) {
        item->colorFilter = lua_toVector4f(L, 4);
    } else if (strcmp(paramName, "nofog") == 0) {
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
	}else if (strcmp(paramName, "nodesCullFrustum") == 0) {
		if (lua_toboolean(L, 4)) {
			item->flags = item->flags | RENDERITEM_FLAG_NODES_CULL_FRUSTUM_TEST;
		} else {
			item->flags = item->flags & ~RENDERITEM_FLAG_NODES_CULL_FRUSTUM_TEST;
		}
	} else if (strcmp(paramName, "nodirlight") == 0) {
		if (lua_toboolean(L, 4)) {
			item->flags = item->flags | RENDERITEM_FLAG_NO_DIR_LIGHT;
		} else {
			item->flags = item->flags & ~RENDERITEM_FLAG_NO_DIR_LIGHT;
		}
	} else if (strcmp(paramName, "noshadowgen") == 0) {
		if (lua_toboolean(L, 4)) {
			item->flags = item->flags | RENDERITEM_FLAG_NO_SHADOW_GEN;
		} else {
			item->flags = item->flags & ~RENDERITEM_FLAG_NO_SHADOW_GEN;
		}
	} else if (strcmp(paramName, "noshadowrecv") == 0) {
		if (lua_toboolean(L, 4)) {
			item->flags = item->flags | RENDERITEM_FLAG_NO_SHADOW_RECV;
		} else {
			item->flags = item->flags & RENDERITEM_FLAG_NO_SHADOW_RECV;
		}
	} else if (strcmp(paramName, "drawfirst") == 0) {
		if (lua_toboolean(L, 4)) {
			item->flags = item->flags | RENDERITEM_FLAG_DRAW_FIRST;
		} else {
			item->flags = item->flags & RENDERITEM_FLAG_DRAW_FIRST;
		}
	} else if (strcmp(paramName, "twosided") == 0) {
		if (lua_toboolean(L, 4)) {
			item->flags = item->flags | RENDERITEM_FLAG_TWO_SIDED;
		} else {
			item->flags = item->flags & RENDERITEM_FLAG_TWO_SIDED;
		}
    } else if (strcmp(paramName, "multiline") == 0) {
        if (lua_toboolean(L, 4)) {
			item->flags = item->flags | RENDERITEM_FLAG_MULTILINE_TEXT;
		} else {
			item->flags = item->flags & RENDERITEM_FLAG_MULTILINE_TEXT;
		}
    } else if (strcmp(paramName, "isOpaque") == 0) {
    	if (!lua_toboolean(L, 4)) {
			item->flags = item->flags & ~RENDERITEM_FLAG_IS_OPAQUE;
		} else {
			item->flags = item->flags | RENDERITEM_FLAG_IS_OPAQUE;
		}
    }
	return 0;
}

// game, tag, text -- returns width/height
static int lua_Game_measureText(lua_State *L) {
	// Game *game = *(Game**)lua_touserdata(L, 1);
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

static int lua_Game_engineReset(lua_State *L) {
	static_context->game->needsReset = TRUE;
	return 0;
}

static int lua_Game_addParticleEmitter(lua_State *L) {
    ParticleEmitter* emitter = new ParticleEmitter();
    static_context->world->emitters->put(emitter->getID(),emitter);
    lua_pushinteger(L, emitter->getID());
    return 1;
}

static int lua_Game_setParticleEmitterTimeRange(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);

    F32 min = (F32)lua_tonumber(L, 3);
    F32 max = (F32)lua_tonumber(L, 4);

    if( emitter )
        emitter->setParticleLifeTimeRange(max, min);

    return 0;
}

static int lua_Game_setParticleEmitterPosition(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);

	Vector3f pos = lua_toVector3f(L, 3);
    if( emitter )
        emitter->setEmitterSourceLocation(pos);

    return 0;
}

static int lua_Game_setParticleEmitterDirection(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);

    if( emitter )
    {
        Vector3f dir = lua_toVector3f(L, 3);
        emitter->setEmissionDirection(dir);
    }
    return 0;
}

static int lua_Game_setParticleEmitterTextureAsset(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);
    if (emitter) {
         emitter->setTextureAssetName(lua_tostring(L, 3));
    }
    return 0;
}

static int lua_Game_removeParticleEmitter(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);
    if( emitter ) {
        emitter->stopEmitter();
        static_context->world->emitters->remove(emitterID);
        delete emitter;
    }

    return 0;
}

static int lua_Game_stopParticleEmitter(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);
    if( emitter ) {
        emitter->stopEmitter();
    }
    return 0;
}

static int lua_Game_clearParticleEmitters(lua_State *L) {
	static_context->world->emitters->deleteElements();
    return 0;
}

static int lua_Game_startParticleEmitter(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);
    if( emitter )
        emitter->startEmitter();

    return 0;
}

static int lua_Game_setParticleEmitterConeRange(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);
    F32 range = lua_tonumber(L, 3);

    if( emitter )
        emitter->setEmissionConeRange(range);

    return 0;
}

static int lua_Game_setParticleScaleSpeedRange(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);

    F32 min = (F32)lua_tonumber(L, 3);
    F32 max = (F32)lua_tonumber(L, 4);

    if( emitter )
        emitter->setParticleScaleSpeedRange(max, min);

    return 0;
}

static int lua_Game_setParticleAlphaSpeedRange(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);

    F32 min = (F32)lua_tonumber(L, 3);
    F32 max = (F32)lua_tonumber(L, 4);

    if( emitter )
        emitter->setParticleAlphaSpeedRange(max, min);

    return 0;
}

static int lua_Game_setParticleMaxSpeedRange(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);

    F32 min = (F32)lua_tonumber(L, 3);
    F32 max = (F32)lua_tonumber(L, 4);

    if( emitter )
        emitter->setParticleMaxSpeedRange(max, min);

    return 0;
}

static int lua_Game_setParticleEmissionRate(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);

    F32 rate = (F32)lua_tonumber(L, 3);
    if( emitter )
        emitter->setEmissionRate(rate);

    return 0;
}

static int lua_Game_setParticleInitialScale(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);

    F32 rate = (F32)lua_tonumber(L, 3);
    if( emitter )
        emitter->setParticleInitialScale(rate);

    return 0;
}

static int lua_Game_setParticleRotationSpeedRange(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);

    F32 min = (F32)lua_tonumber(L, 3);
    F32 max = (F32)lua_tonumber(L, 4);

    if( emitter )
        emitter->setParticleRotationSpeedRange(max, min);

    return 0;
}

static int lua_Game_setParticleGravity(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);
    
    F32 gravity = (F32)lua_tonumber(L, 3);
    
    if( emitter )
        emitter->setGravity(gravity);

    return 0;
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
