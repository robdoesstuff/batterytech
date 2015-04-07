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
#ifdef BATTERYTECH_INCLUDE_BOX2D
#include <bt-box2d/Dynamics/Joints/b2DistanceJoint.h>
#include <bt-box2d/Dynamics/Joints/b2MouseJoint.h>
#include <bt-box2d/Dynamics/Joints/b2RevoluteJoint.h>
#include <bt-box2d/Dynamics/Joints/b2PrismaticJoint.h>
#include <bt-box2d/Dynamics/Joints/b2RopeJoint.h>
#include <bt-box2d/Dynamics/Joints/b2PulleyJoint.h>
#include <bt-box2d/Dynamics/Joints/b2GearJoint.h>
#include <bt-box2d/Dynamics/Joints/b2WheelJoint.h>
#include <bt-box2d/Dynamics/Joints/b2WeldJoint.h>
#include <bt-box2d/Dynamics/Joints/b2FrictionJoint.h>
#endif

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
static int lua_Game_clearPhysicsForces(lua_State *l);
static int lua_Game_setPhysicsGravity(lua_State *L);
static int lua_Game_setPhysicsDrawDebug(lua_State *L);
static int lua_Game_queryPhysicsAABB(lua_State *L);
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
static int lua_Game_start2DProjection(lua_State *L);
static int lua_Game_end2DProjection(lua_State *L);
static int lua_Game_setShadowLightOrigin(lua_State *L);
static int lua_Game_setShadowColorAndEpsilon(lua_State *L);
static int lua_Game_setShadowLightFrustumNearFar(lua_State *L);
static int lua_Game_setShadowOrtho(lua_State *L);
static int lua_Game_setShadowPerspective(lua_State *L);
static int lua_Game_setShadowCullMode(lua_State *L);
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
static int lua_Game_add2DParticleEmitter(lua_State *L);
static int lua_Game_setParticleEmitterTimeRange(lua_State *L);
static int lua_Game_setParticleEmitterPosition(lua_State *L);
static int lua_Game_setParticleEmitterPositionRange(lua_State *L);
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
static int lua_Game_setParticleAutoStopMax(lua_State *L);
#ifdef BATTERYTECH_INCLUDE_BOX2D
static int lua_Game_physics_addDistanceJoint(lua_State *L);
static int lua_Game_physics_addRevoluteJoint(lua_State *L);
static int lua_Game_physics_setRevoluteJointLimits(lua_State *L);
static int lua_Game_physics_setRevoluteJointMotor(lua_State *L);
static int lua_Game_physics_getRevoluteJointValues(lua_State *L);
static int lua_Game_physics_addMouseJoint(lua_State *L);
static int lua_Game_physics_setMouseJointPosition(lua_State *L);
static int lua_Game_physics_addPrismaticJoint(lua_State *L);
static int lua_Game_physics_setPrismaticJointLimits(lua_State *L);
static int lua_Game_physics_setPrismaticJointMotor(lua_State *L);
static int lua_Game_physics_getPrismaticJointValues(lua_State *L);
static int lua_Game_physics_addRopeJoint(lua_State *L);
static int lua_Game_physics_addPulleyJoint(lua_State *L);
static int lua_Game_physics_getPulleyJointValues(lua_State *L);
static int lua_Game_physics_addGearJoint(lua_State *L);
static int lua_Game_physics_addWheelJoint(lua_State *L);
static int lua_Game_physics_setWheelJointMotor(lua_State *L);
static int lua_Game_physics_getWheelJointValues(lua_State *L);
static int lua_Game_physics_addWeldJoint(lua_State *L);
static int lua_Game_physics_addFrictionJoint(lua_State *L);
static int lua_Game_physics_removeJoint(lua_State *L);
static int lua_Game_physics_getJointAnchorPoints(lua_State *L);
#endif

static const luaL_reg lua_methods[] = {
	{ "getInstance", lua_Game_getInstance },
	{ "replaceMetatable", lua_Game_replaceMetatable },
	{ "createPhysicsWorld", lua_Game_createPhysicsWorld },
	{ "clearPhysicsWorld", lua_Game_clearPhysicsWorld },
	{ "destroyPhysicsWorld", lua_Game_destroyPhysicsWorld },
	{ "updatePhysics", lua_Game_updatePhysics },
	{ "clearPhysicsForces", lua_Game_clearPhysicsForces },
	{ "setPhysicsGravity", lua_Game_setPhysicsGravity },
	{ "setPhysicsDrawDebug", lua_Game_setPhysicsDrawDebug },
	{ "queryPhysicsAABB", lua_Game_queryPhysicsAABB },
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
    { "start2DProjection", lua_Game_start2DProjection },
    { "end2DProjection", lua_Game_end2DProjection },
	{ "loadNewResources", lua_Game_loadNewResources },
	{ "getShadowType", lua_Game_getShadowType },
	{ "setShadowType", lua_Game_setShadowType },
	{ "setShadowLightOrigin", lua_Game_setShadowLightOrigin },
	{ "setShadowColorAndEpsilon", lua_Game_setShadowColorAndEpsilon },
	{ "setShadowLightFrustumNearFar", lua_Game_setShadowLightFrustumNearFar },
	{ "setShadowOrtho", lua_Game_setShadowOrtho },
	{ "setShadowPerspective", lua_Game_setShadowPerspective },
	{ "setShadowCullMode", lua_Game_setShadowCullMode },
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
    { "add2DParticleEmitter", lua_Game_add2DParticleEmitter },
    { "setParticleEmitterTimeRange", lua_Game_setParticleEmitterTimeRange },
    { "setParticleEmitterPosition", lua_Game_setParticleEmitterPosition },
    { "setParticleEmitterPositionRange", lua_Game_setParticleEmitterPositionRange },
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
    { "setParticleAutoStopMax", lua_Game_setParticleAutoStopMax },
#ifdef BATTERYTECH_INCLUDE_BOX2D
    { "physics_addDistanceJoint", lua_Game_physics_addDistanceJoint },
    { "physics_addRevoluteJoint", lua_Game_physics_addRevoluteJoint },
    { "physics_setRevoluteJointLimits", lua_Game_physics_setRevoluteJointLimits },
    { "physics_setRevoluteJointMotor", lua_Game_physics_setRevoluteJointMotor },
    { "physics_getRevoluteJointValues", lua_Game_physics_getRevoluteJointValues },
    { "physics_addMouseJoint", lua_Game_physics_addMouseJoint },
    { "physics_setMouseJointPosition", lua_Game_physics_setMouseJointPosition },
    { "physics_addPrismaticJoint", lua_Game_physics_addPrismaticJoint },
    { "physics_setPrismaticJointLimits", lua_Game_physics_setPrismaticJointLimits },
    { "physics_setPrismaticJointMotor", lua_Game_physics_setPrismaticJointMotor },
    { "physics_getPrismaticJointValues", lua_Game_physics_getPrismaticJointValues },
    { "physics_addRopeJoint", lua_Game_physics_addRopeJoint },
    { "physics_addPulleyJoint", lua_Game_physics_addPulleyJoint },
    { "physics_getPulleyJointValues", lua_Game_physics_getPulleyJointValues },
    { "physics_addGearJoint", lua_Game_physics_addGearJoint },
    { "physics_addWheelJoint", lua_Game_physics_addWheelJoint },
    { "physics_setWheelJointMotor", lua_Game_physics_setWheelJointMotor },
    { "physics_getWheelJointValues", lua_Game_physics_getWheelJointValues },
    { "physics_addWeldJoint", lua_Game_physics_addWeldJoint },
    { "physics_addFrictionJoint", lua_Game_physics_addFrictionJoint },
    { "physics_removeJoint", lua_Game_physics_removeJoint },
    { "physics_getJointAnchorPoints", lua_Game_physics_getJointAnchorPoints },
#endif
	{ 0, 0 } };

static const luaL_reg metatable[] = {
	{"__gc",       lua_Game_gc},
	{"__tostring", lua_Game_tostring},
	{0, 0}
};

static void initRenderItemFunctions();

#ifdef BATTERYTECH_INCLUDE_BOX2D
static int nextJointID = 0;
#endif

GameLuaBinder::GameLuaBinder(lua_State *L, Game *game, GameContext *context) :
	LuaBinder(L) {
	this->game = game;
	this->context = context;
	gameRef = 0;
    initRenderItemFunctions();
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
    if (static_context->world->boxWorld) {
        logmsg("Error - 2D Physics already exists");
    } else {
        b2Vec2 gravity(0.0f, DEFAULT_GRAVITY);
        static_context->world->boxWorld = new b2World(gravity);
        static_context->world->boxWorld->SetContactListener(game);
        static_context->world->boxWorld->SetDestructionListener(game);
    }
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

static int lua_Game_clearPhysicsForces(lua_State *L) {
#ifdef BATTERYTECH_INCLUDE_BOX2D
    static_context->world->boxWorld->ClearForces();
#endif
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

// boolean (on/off), then for 2D - optional projection left, right, bottom, top, near, far
static int lua_Game_setPhysicsDrawDebug(lua_State *L) {
	// Game *game = *(Game**)lua_touserdata(L, 1);
	static_context->world->physicsDrawDebug = lua_toboolean(L, 2);
    if (lua_isnumber(L, 3) && lua_isnumber(L, 8)) {
        static_context->world->physicsDrawDebugUsingProjection = TRUE;
        for (S32 i = 0; i < 6; i++) {
            static_context->world->physicsDrawDebugProjection[i] = lua_tonumber(L, i+3);
        }
    } else {
        static_context->world->physicsDrawDebugUsingProjection = FALSE;
    }
	return 0;
}

static int lua_Game_queryPhysicsAABB(lua_State *L) {
	// Game *game = *(Game**)lua_touserdata(L, 1);
#ifdef BATTERYTECH_INCLUDE_BOX2D
	if (!static_context->world->boxWorld) {
		return 0;
	}
	F32 x1,y1,x2,y2;
	x1 = lua_tonumber(L, 2);
	y1 = lua_tonumber(L, 3);
	x2 = lua_tonumber(L, 4);
	y2 = lua_tonumber(L, 5);
	LuaBinderQueryCallback callback(L);
	b2AABB aabb;
	aabb.lowerBound.Set(x1, y1);
	aabb.upperBound.Set(x2, y2);
	static_context->world->boxWorld->QueryAABB(&callback, aabb);
	return callback.returnCount;
#else
	return 0;
#endif
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

	if (lua_isnumber(L, 11) && lua_isnumber(L, 12) && lua_isnumber(L, 13)) {
		Vector3f scale = lua_toVector3f(L,11);
		item->mat.scale(scale.x,scale.y,scale.z);
	}

	if (lua_isnumber(L, 14) && lua_isnumber(L, 15) && lua_isnumber(L, 16)) {
		item->mat.rotate(lua_tonumber(L, 14), 1, 0, 0);
		item->mat.rotate(lua_tonumber(L, 15), 0, 1, 0);
		item->mat.rotate(lua_tonumber(L, 16), 0, 0, 1);
	}
	item->mat.setTranslation(translation);
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
	if (lua_gettop(L) >= 5) {
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
	if (!assetName) {
		logmsg("render2D(nil, ...) called - invalid");
		return 0;
	}
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
	if (!assetName) {
		logmsg("render2DBG(nil, ...) called - invalid");
		return 0;
	}
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
	if (!assetName) {
		logmsg("renderBB(nil, ...) called - invalid");
		return 0;
	}
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

// left, right, bottom, top, near, far
static int lua_Game_start2DProjection(lua_State *L) {
    //Game *game = *(Game**)lua_touserdata(L, 1);
	if (static_context->world->renderItemsUsed == MAX_RENDERITEMS) {
		lua_pushinteger(L, -1);
		return 1;
	}
	RenderItem *item = &static_context->world->renderItems[static_context->world->renderItemsUsed++];
	item->reset();
	item->renderType = RenderItem::RENDERTYPE_2DPROJ;
    Vector4f lrbt = lua_toVector4f(L, 2);
    Vector2f nf = lua_toVector2f(L, 6);
    // values packed into matrix, left, right, bottom, top, near, far
    Matrix4f *mat = &item->mat;
    mat->data[0] = lrbt.x;
    mat->data[1] = lrbt.y;
    mat->data[2] = lrbt.z;
    mat->data[3] = lrbt.w;
    mat->data[4] = nf.x;
    mat->data[5] = nf.y;
	lua_pushinteger(L, static_context->world->renderItemsUsed-1);
	return 1;    
}

static int lua_Game_end2DProjection(lua_State *L) {
	if (static_context->world->renderItemsUsed == MAX_RENDERITEMS) {
		lua_pushinteger(L, -1);
		return 1;
	}
	RenderItem *item = &static_context->world->renderItems[static_context->world->renderItemsUsed++];
	item->reset();
	item->renderType = RenderItem::RENDERTYPE_2DPROJ;
    // values packed into matrix, left, right, bottom, top, near, far
    Matrix4f *mat = &item->mat;
    GraphicsConfiguration *gConfig = static_context->gConfig;
    // reset to default values (screen pixel ortho with 1, -1 z range)
    mat->data[0] = 0;
    mat->data[1] = gConfig->width;
    mat->data[2] = gConfig->height;
    mat->data[3] = 0;
    mat->data[4] = -1;
    mat->data[5] = 1;
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

static int lua_Game_setShadowCullMode(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
    static_context->world->globalLight->shadowCullMode = lua_tonumber(L, 2);
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
	} else if (sType == 3) {
		// params 3 & 4 are custom height and width
		static_context->gConfig->shadowType = GraphicsConfiguration::SHADOWTYPE_SHADOWMAP_CUSTOM;
		Vector2f *size = (Vector2f*)static_context->renderContext->userValues->get("shadowmap_size");
		if (!size) {
			size = new Vector2f(lua_tonumber(L, 3), lua_tonumber(L, 4));
			static_context->renderContext->userValues->put("shadowmap_size", size);
		} else {
			size->x = lua_tonumber(L, 3);
			size->y = lua_tonumber(L, 4);
		}
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
			aiVector3D avmin(999999.0, 999999.0, 999999.0);
			aiVector3D avmax(-999999.0, -999999.0, -999999.0);
			for (U32 j = 0; j < mesh->mNumVertices; j++) {
				aiVector3D vert = mesh->mVertices[j];
				centerPoint += vert;
				if (vert.x < avmin.x) { avmin.x = vert.x; }
				if (vert.y < avmin.y) { avmin.y = vert.y; }
				if (vert.x < avmin.z) { avmin.z = vert.z; }
				if (vert.x > avmax.x) { avmax.x = vert.x; }
				if (vert.y > avmax.y) { avmax.y = vert.y; }
				if (vert.x > avmax.z) { avmax.z = vert.z; }
			}
			centerPoint /= mesh->mNumVertices;
			centerPoint *= transform;
			Vector3f pos(centerPoint.x, centerPoint.y, centerPoint.z);
			Vector3f vMin(avmin.x, avmin.y, avmin.z);
			Vector3f vMax(avmax.x, avmax.y, avmax.z);
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

//-------------------- RENDERITEM PARAM Start --------------------

static int setRenderItemParam_uvs(lua_State *L, RenderItem *item) {
    item->uvs = lua_toVector4f(L, 4);
    return 0;
}

static int setRenderItemParam_align(lua_State *L, RenderItem *item) {
    const char *paramValue = lua_tostring(L, 4);
    if (strcmp(paramValue, "left") == 0) {
        item->alignment = RenderItem::ALIGN_LEFT;
    } else if (strcmp(paramValue, "center") == 0) {
        item->alignment = RenderItem::ALIGN_CENTER;
    } else if (strcmp(paramValue, "right") == 0) {
        item->alignment = RenderItem::ALIGN_RIGHT;
    }
    return 0;
}

static int setRenderItemParam_alpha(lua_State *L, RenderItem *item) {
    item->colorFilter.a = lua_tonumber(L, 4);
    return 0;
}

static int setRenderItemParam_colorFilter(lua_State *L, RenderItem *item) {
    item->colorFilter = lua_toVector4f(L, 4);
    return 0;
}

static int setRenderItemParam_nofog(lua_State *L, RenderItem *item) {
    if (lua_toboolean(L, 4)) {
        item->flags = item->flags | RENDERITEM_FLAG_NO_FOG;
    } else {
        item->flags = item->flags & ~RENDERITEM_FLAG_NO_FOG;
    }
    return 0;
}

static int setRenderItemParam_maxPointLights(lua_State *L, RenderItem *item) {
    item->maxPointLights = lua_tointeger(L, 4);
    return 0;
}

static int setRenderItemParam_useCameraClosestPointLights(lua_State *L, RenderItem *item) {
    if (lua_toboolean(L, 4)) {
        item->flags = item->flags | RENDERITEM_FLAG_USE_CAMERA_CLOSEST_POINT_LIGHTS;
    } else {
        item->flags = item->flags & ~RENDERITEM_FLAG_USE_CAMERA_CLOSEST_POINT_LIGHTS;
    }
    return 0;
}

static int setRenderItemParam_viewport(lua_State *L, RenderItem *item) {
    item->viewport = lua_toVector4i(L, 4);
    return 0;
}

static int setRenderItemParam_nodesCullFrustum(lua_State *L, RenderItem *item) {
    if (lua_toboolean(L, 4)) {
        item->flags = item->flags | RENDERITEM_FLAG_NODES_CULL_FRUSTUM_TEST;
    } else {
        item->flags = item->flags & ~RENDERITEM_FLAG_NODES_CULL_FRUSTUM_TEST;
    }
    return 0;
}

static int setRenderItemParam_nodirlight(lua_State *L, RenderItem *item) {
    if (lua_toboolean(L, 4)) {
        item->flags = item->flags | RENDERITEM_FLAG_NO_DIR_LIGHT;
    } else {
        item->flags = item->flags & ~RENDERITEM_FLAG_NO_DIR_LIGHT;
    }
    return 0;
}

static int setRenderItemParam_noshadowgen(lua_State *L, RenderItem *item) {
    if (lua_toboolean(L, 4)) {
        item->flags = item->flags | RENDERITEM_FLAG_NO_SHADOW_GEN;
    } else {
        item->flags = item->flags & ~RENDERITEM_FLAG_NO_SHADOW_GEN;
    }
    return 0;
}

static int setRenderItemParam_noshadowrecv(lua_State *L, RenderItem *item) {
    if (lua_toboolean(L, 4)) {
        item->flags = item->flags | RENDERITEM_FLAG_NO_SHADOW_RECV;
    } else {
        item->flags = item->flags & RENDERITEM_FLAG_NO_SHADOW_RECV;
    }
    return 0;
}

static int setRenderItemParam_drawfirst(lua_State *L, RenderItem *item) {
    if (lua_toboolean(L, 4)) {
        item->flags = item->flags | RENDERITEM_FLAG_DRAW_FIRST;
    } else {
        item->flags = item->flags & RENDERITEM_FLAG_DRAW_FIRST;
    }
    return 0;
}

static int setRenderItemParam_twosided(lua_State *L, RenderItem *item) {
    if (lua_toboolean(L, 4)) {
        item->flags = item->flags | RENDERITEM_FLAG_TWO_SIDED;
    } else {
        item->flags = item->flags & RENDERITEM_FLAG_TWO_SIDED;
    }
    return 0;
}

static int setRenderItemParam_multiline(lua_State *L, RenderItem *item) {
    if (lua_toboolean(L, 4)) {
        item->flags = item->flags | RENDERITEM_FLAG_MULTILINE_TEXT;
    } else {
        item->flags = item->flags & RENDERITEM_FLAG_MULTILINE_TEXT;
    }
    return 0;
}

static int setRenderItemParam_isOpaque(lua_State *L, RenderItem *item) {
    if (!lua_toboolean(L, 4)) {
        item->flags = item->flags & ~RENDERITEM_FLAG_IS_OPAQUE;
    } else {
        item->flags = item->flags | RENDERITEM_FLAG_IS_OPAQUE;
    }
    return 0;
}

static int setRenderItemParam_lightperpixel(lua_State *L, RenderItem *item) {
    if (!lua_toboolean(L, 4)) {
        item->flags = item->flags & ~RENDERITEM_FLAG_LIGHT_PER_PIXEL;
    } else {
        item->flags = item->flags | RENDERITEM_FLAG_LIGHT_PER_PIXEL;
    }
    return 0;
}

static int setRenderItemParam_maxtextsize(lua_State *L, RenderItem *item) {
	item->scale.y = lua_tonumber(L, 4);
	item->scale.z = lua_tonumber(L, 5);
	return 0;
}

#define RENDERITEMFUNC int(*)(lua_State*, RenderItem*)

static StrHashTable<RENDERITEMFUNC> *renderItemFuncs = new StrHashTable<RENDERITEMFUNC>();

void initRenderItemFunctions() {
    if (renderItemFuncs->size()) {
        // already initialized
        return;
    }
    renderItemFuncs->put("uvs", setRenderItemParam_uvs);
    renderItemFuncs->put("align", setRenderItemParam_align);
    renderItemFuncs->put("alpha", setRenderItemParam_alpha);
    renderItemFuncs->put("colorFilter", setRenderItemParam_colorFilter);
    renderItemFuncs->put("nofog", setRenderItemParam_nofog);
    renderItemFuncs->put("maxPointLights", setRenderItemParam_maxPointLights);
    renderItemFuncs->put("useCameraClosestPointLights", setRenderItemParam_useCameraClosestPointLights);
    renderItemFuncs->put("viewport", setRenderItemParam_viewport);
    renderItemFuncs->put("nodesCullFrustum", setRenderItemParam_nodesCullFrustum);
    renderItemFuncs->put("nodirlight", setRenderItemParam_nodirlight);
    renderItemFuncs->put("noshadowgen", setRenderItemParam_noshadowgen);
    renderItemFuncs->put("noshadowrecv", setRenderItemParam_noshadowrecv);
    renderItemFuncs->put("drawfirst", setRenderItemParam_drawfirst);
    renderItemFuncs->put("twosided", setRenderItemParam_twosided);
    renderItemFuncs->put("multiline", setRenderItemParam_multiline);
    renderItemFuncs->put("isOpaque", setRenderItemParam_isOpaque);
    renderItemFuncs->put("lightperpixel", setRenderItemParam_lightperpixel);
    renderItemFuncs->put("maxtextsize", setRenderItemParam_maxtextsize);
}

// game, renderItemIdx, paramName, values...
static int lua_Game_setRenderItemParam(lua_State *L) {
	//Game *game = *(Game**)lua_touserdata(L, 1);
	S32 idx = lua_tointeger(L, 2);
	RenderItem *item = &static_context->world->renderItems[idx];
	const char *paramName = lua_tostring(L, 3);
    // we hashed the key of param setting code so that we could expand the list without significant computational cost
    // of using a key string
    int(*func)(lua_State*, RenderItem*) = renderItemFuncs->get(paramName);
    if (func) {
        return func(L, item);
    }
	return 0;
}

//-------------------- RENDERITEM PARAM End --------------------


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

static int lua_Game_add2DParticleEmitter(lua_State *L) {
    ParticleEmitter* emitter = new ParticleEmitter();
    emitter->is2D = TRUE;
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
    if( emitter ) {
	   if (emitter->is2D) {
		   Vector2f loc2D = lua_toVector2f(L, 3);
		   emitter->setEmitterSourceLocation(Vector3f(loc2D.x, loc2D.y, 0));
	   } else {
		   emitter->setEmitterSourceLocation(lua_toVector3f(L, 3));
	   }
    }
    return 0;
}

static int lua_Game_setParticleEmitterPositionRange(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);
    if( emitter ) {
 	   if (emitter->is2D) {
 		   Vector2f range2D = lua_toVector2f(L, 3);
 		   emitter->setPositionRange(Vector3f(range2D.x, range2D.y, 0));
 	   } else {
 		   emitter->setPositionRange(lua_toVector3f(L, 3));
 	   }
    }
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
    if( emitter ) {
        emitter->startEmitter();
    }
    return 0;
}

static int lua_Game_setParticleEmitterConeRange(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);
    F32 range = lua_tonumber(L, 3);

    if( emitter ) {
        emitter->setEmissionConeRange(range);
    }
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

    if( emitter ) {
        emitter->setParticleMaxSpeedRange(max, min);
    }
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

    F32 scale = (F32)lua_tonumber(L, 3);
    if( emitter ) {
        emitter->setParticleInitialScale(scale);
    }
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
    if (!emitter) {
    	return 0;
    }
    Vector3f newGravity(0,0,0);
    if (lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
    	// 3D gravity vector
    	newGravity = lua_toVector3f(L, 3);
    } else if (lua_isnumber(L, 3) && lua_isnumber(L, 4)) {
    	// 2D gravity vector
    	F32 x = lua_tonumber(L, 3);
    	F32 y = lua_tonumber(L, 4);
    	newGravity.x = x;
    	newGravity.y = y;
    } else {
    	// 1D gravity vector (Z axis for 3D, Y axis for 2D)
    	F32 gravity = (F32)lua_tonumber(L, 3);
    	if (emitter->is2D) {
    		newGravity.y = gravity;
    	} else {
    		newGravity.z = gravity;
    	}
    }
    emitter->setGravity(newGravity);
    return 0;
}

static int lua_Game_setParticleAutoStopMax(lua_State *L) {
	S32 emitterID = lua_tointeger(L, 2);
    ParticleEmitter* emitter = static_context->world->emitters->get(emitterID);
    if( emitter ) {
       S32 max = lua_tointeger(L, 3);
       emitter->setAutoStopMax(max);
    }
	return 0;
}

#ifdef BATTERYTECH_INCLUDE_BOX2D
static b2Body* getBody(GameObject *obj, S32 idx) {
	return obj->boxBodies->array[idx];
}

static int lua_Game_physics_addDistanceJoint(lua_State *L) {
    // param 1 is game
    // param 2 is gameobject1, 3 is index1
    // param 4 is gameobject2, 5 is index2
    // params 6,7 are anchorA X,Y
    // params 8,9 are anchorB X,Y
    // param 10 (optional) is collideConnected
    // param 11 (optional) is frequencyHz
    // param 12 (optional) is dampingRatio
    GameObject *objA = *(GameObject**)lua_touserdata(L, 2);
    S32 idxA = lua_tointeger(L, 3);
    GameObject *objB = *(GameObject**)lua_touserdata(L, 4);
    S32 idxB = lua_tointeger(L, 5);
    b2Body *myBodyA = getBody(objA, idxA);
    b2Body *myBodyB = getBody(objB, idxB);
    b2Vec2 worldAnchorOnBodyA = b2Vec2(lua_tonumber(L, 6), lua_tonumber(L, 7));
    b2Vec2 worldAnchorOnBodyB = b2Vec2(lua_tonumber(L, 8), lua_tonumber(L, 9));
    S32 jointID = nextJointID++;
    b2DistanceJointDef jointDef;
    jointDef.Initialize(myBodyA, myBodyB, worldAnchorOnBodyA, worldAnchorOnBodyB);
    if (lua_isboolean(L, 10)) {
        jointDef.collideConnected = lua_toboolean(L, 10);
        if (lua_isnumber(L, 11)) {
            jointDef.frequencyHz = lua_tonumber(L, 11);
            if (lua_isnumber(L, 12)) {
                jointDef.dampingRatio = lua_tonumber(L, 12);
            }
        }
    }
    b2Joint *joint = static_context->world->boxWorld->CreateJoint(&jointDef);
    joint->SetUserData((void*)jointID);
    static_context->world->boxJoints->put(jointID, joint);
    lua_pushinteger(L, jointID);
    return 1;
}

static int lua_Game_physics_addRevoluteJoint(lua_State *L) {
    // param 1 is game
    // param 2 is gameobject1, 3 is index1
    // param 4 is gameobject2, 5 is index2
    // params 6,7 are anchorA X,Y
    GameObject *objA = *(GameObject**)lua_touserdata(L, 2);
    S32 idxA = lua_tointeger(L, 3);
    GameObject *objB = *(GameObject**)lua_touserdata(L, 4);
    S32 idxB = lua_tointeger(L, 5);
    b2Body *myBodyA = getBody(objA, idxA);
    b2Body *myBodyB = getBody(objB, idxB);
    b2Vec2 worldAnchorOnBodyA = b2Vec2(lua_tonumber(L, 6), lua_tonumber(L, 7));
    S32 jointID = nextJointID++;
    b2RevoluteJointDef jointDef;
    jointDef.Initialize(myBodyA, myBodyB, worldAnchorOnBodyA);
    b2Joint *joint = static_context->world->boxWorld->CreateJoint(&jointDef);
    joint->SetUserData((void*)jointID);
    static_context->world->boxJoints->put(jointID, joint);
    lua_pushinteger(L, jointID);
    return 1;
}

static int lua_Game_physics_setRevoluteJointLimits(lua_State *L) {
    // param 1 is game
	// param 2 is jointID
	// param 3 is limits enabled
	// param 4 is limit low (opt)
	// param 5 is limit high (opt)
	S32 jointID = lua_tointeger(L, 2);
	if (static_context->world->boxJoints) {
		b2Joint *joint = static_context->world->boxJoints->get(jointID);
		if (!joint) {
			char buf[255];
			sprintf(buf, "No revolute joint found for ID %d", jointID);
			logmsg(buf);
		} else {
			if (joint->GetType() == e_revoluteJoint) {
				b2RevoluteJoint *rJoint = (b2RevoluteJoint*) joint;
				rJoint->EnableLimit(lua_toboolean(L, 3));
				if (lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
					rJoint->SetLimits(lua_tonumber(L, 4), lua_tonumber(L, 5));
				}
			} else {
				logmsg("physics_setRevoluteJointLimits called on non-revolute joint");
			}
		}
	}
	return 0;
}

static int lua_Game_physics_setRevoluteJointMotor(lua_State *L) {
    // param 1 is game
	// param 2 is jointID
	// param 3 is motor enabled
	// param 4 is motor speed (opt)
	// param 5 is max motor torque (opt)
	S32 jointID = lua_tointeger(L, 2);
	if (static_context->world->boxJoints) {
		b2Joint *joint = static_context->world->boxJoints->get(jointID);
		if (!joint) {
			char buf[255];
			sprintf(buf, "No revolute joint found for ID %d", jointID);
			logmsg(buf);
		} else {
			if (joint->GetType() == e_revoluteJoint) {
				b2RevoluteJoint *rJoint = (b2RevoluteJoint*) joint;
				rJoint->EnableMotor(lua_toboolean(L, 3));
				if (lua_isnumber(L, 4)) {
					rJoint->SetMotorSpeed(lua_tonumber(L, 4));
				}
				if (lua_isnumber(L, 5)) {
					rJoint->SetMaxMotorTorque(lua_tonumber(L, 5));
				}
			} else {
				logmsg("physics_setRevoluteJointMotor called on non-revolute joint");
			}
		}
	}
	return 0;
}

static int lua_Game_physics_getRevoluteJointValues(lua_State *L) {
	// param 1 is game
	// param 2 is joint ID
	// returns angle, speed and torque
	S32 jointID = lua_tointeger(L, 2);
	b2Joint *joint = static_context->world->boxJoints->get(jointID);
	if (!joint) {
		char buf[255];
		sprintf(buf, "No revolute joint found for ID %d", jointID);
		logmsg(buf);
	} else {
		if (joint->GetType() == e_revoluteJoint) {
			b2RevoluteJoint *rJoint = (b2RevoluteJoint*) joint;
			lua_pushnumber(L, rJoint->GetJointAngle());
			lua_pushnumber(L, rJoint->GetJointSpeed());
			lua_pushnumber(L, rJoint->GetMotorTorque(1.0f));
			return 3;
		} else {
			logmsg("physics_getRevoluteJointValues called on non-revolute joint");
		}
	}
	return 0;
}

static int lua_Game_physics_addMouseJoint(lua_State *L) {
    // param 1 is game
    // param 2 is gameobject1, 3 is index1
    // param 4 is gameobject2, 5 is index2
    // params 6,7 are anchorA X,Y
	// param 8 (optional) is maxForce
    // param 9 (optional) is frequencyHz
    // param 10 (optional) is dampingRatio
    GameObject *objA = *(GameObject**)lua_touserdata(L, 2);
    S32 idxA = lua_tointeger(L, 3);
    GameObject *objB = *(GameObject**)lua_touserdata(L, 4);
    S32 idxB = lua_tointeger(L, 5);
    b2Vec2 worldAnchorOnBodyA = b2Vec2(lua_tonumber(L, 6), lua_tonumber(L, 7));
    S32 jointID = nextJointID++;
    b2MouseJointDef jointDef;
    jointDef.target = worldAnchorOnBodyA;
    jointDef.bodyA = getBody(objA, idxA);
    jointDef.bodyB = getBody(objB, idxB);
    jointDef.collideConnected = true;
    if (lua_isnumber(L, 8)) {
		jointDef.maxForce = lua_tonumber(L, 8);
		if (lua_isnumber(L, 9)) {
            jointDef.frequencyHz = lua_tonumber(L, 9);
            if (lua_isnumber(L, 10)) {
                jointDef.dampingRatio = lua_tonumber(L, 10);
            }
        }
    } else {
        jointDef.maxForce = 1000 * jointDef.bodyB->GetMass();
    }
    b2Joint *joint = static_context->world->boxWorld->CreateJoint(&jointDef);
    joint->SetUserData((void*)jointID);
    static_context->world->boxJoints->put(jointID, joint);
    lua_pushinteger(L, jointID);
    return 1;
}


static int lua_Game_physics_setMouseJointPosition(lua_State *L) {
    // param 1 is game
	// param 2 is jointID
	// param 3 and 4 are position
	S32 jointID = lua_tointeger(L, 2);
	if (static_context->world->boxJoints) {
		b2Joint *joint = static_context->world->boxJoints->get(jointID);
		if (!joint) {
			char buf[255];
			sprintf(buf, "No mouse joint found for ID %d", jointID);
			logmsg(buf);
		} else {
			if (joint->GetType() == e_mouseJoint) {
				b2MouseJoint *mJoint = (b2MouseJoint*) joint;
				mJoint->SetTarget(b2Vec2(lua_tonumber(L, 3), lua_tonumber(L, 4)));
			} else {
				logmsg("physics_setMouseJointPosition called on non-mouse joint");
			}
		}
	}
	return 0;
}

static int lua_Game_physics_addPrismaticJoint(lua_State *L) {
    // param 1 is game
    // param 2 is gameobject1, 3 is index1
    // param 4 is gameobject2, 5 is index2
    // params 6,7 are anchor X,Y
    // params 8,9 are axis X,Y
    // param 10 (optional) is collideConnected
    GameObject *objA = *(GameObject**)lua_touserdata(L, 2);
    S32 idxA = lua_tointeger(L, 3);
    GameObject *objB = *(GameObject**)lua_touserdata(L, 4);
    S32 idxB = lua_tointeger(L, 5);
    b2Body *myBodyA = getBody(objA, idxA);
    b2Body *myBodyB = getBody(objB, idxB);
    b2Vec2 worldAnchor = b2Vec2(lua_tonumber(L, 6), lua_tonumber(L, 7));
    b2Vec2 axis = b2Vec2(lua_tonumber(L, 8), lua_tonumber(L, 9));
    S32 jointID = nextJointID++;
    b2PrismaticJointDef jointDef;
    jointDef.Initialize(myBodyA, myBodyB, worldAnchor, axis);
    if (lua_isboolean(L, 10)) {
        jointDef.collideConnected = lua_toboolean(L, 10);
    }
    b2Joint *joint = static_context->world->boxWorld->CreateJoint(&jointDef);
    joint->SetUserData((void*)jointID);
    static_context->world->boxJoints->put(jointID, joint);
    lua_pushinteger(L, jointID);
    return 1;
}

static int lua_Game_physics_setPrismaticJointLimits(lua_State *L) {
    // param 1 is game
	// param 2 is jointID
	// param 3 is limits enabled
	// param 4 is limit low (opt)
	// param 5 is limit high (opt)
	S32 jointID = lua_tointeger(L, 2);
	if (static_context->world->boxJoints) {
		b2Joint *joint = static_context->world->boxJoints->get(jointID);
		if (!joint) {
			char buf[255];
			sprintf(buf, "No prismatic joint found for ID %d", jointID);
			logmsg(buf);
		} else {
		   if (joint->GetType() == e_prismaticJoint) {
				b2PrismaticJoint *rJoint = (b2PrismaticJoint*) joint;
				rJoint->EnableLimit(lua_toboolean(L, 3));
				if (lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
					rJoint->SetLimits(lua_tonumber(L, 4), lua_tonumber(L, 5));
				}
			} else {
				logmsg("physics_setPrismaticJointLimits called on non-prismatic joint");
			}
		}
	}
	return 0;
}

static int lua_Game_physics_setPrismaticJointMotor(lua_State *L) {
    // param 1 is game
	// param 2 is jointID
	// param 3 is motor enabled
	// param 4 is motor speed (opt)
	// param 5 is max motor force (opt)
	S32 jointID = lua_tointeger(L, 2);
	if (static_context->world->boxJoints) {
		b2Joint *joint = static_context->world->boxJoints->get(jointID);
		if (!joint) {
			char buf[255];
			sprintf(buf, "No prismatic joint found for ID %d", jointID);
			logmsg(buf);
		} else {
			if (joint->GetType() == e_prismaticJoint) {
				b2PrismaticJoint *rJoint = (b2PrismaticJoint*) joint;
				rJoint->EnableMotor(lua_toboolean(L, 3));
				if (lua_isnumber(L, 4)) {
					rJoint->SetMotorSpeed(lua_tonumber(L, 4));
				}
				if (lua_isnumber(L, 5)) {
					rJoint->SetMaxMotorForce(lua_tonumber(L, 5));
				}
			} else {
				logmsg("physics_setPrismaticJointMotor called on non-prismatic joint");
			}
		}
	}
	return 0;
}

static int lua_Game_physics_getPrismaticJointValues(lua_State *L) {
	// param 1 is game
	// param 2 is joint ID
	// returns translation, speed and force
	S32 jointID = lua_tointeger(L, 2);
	b2Joint *joint = static_context->world->boxJoints->get(jointID);
	if (!joint) {
		char buf[255];
		sprintf(buf, "No prismatic joint found for ID %d", jointID);
		logmsg(buf);
	} else {
		if (joint->GetType() == e_prismaticJoint) {
			b2PrismaticJoint *rJoint = (b2PrismaticJoint*) joint;
			lua_pushnumber(L, rJoint->GetJointTranslation());
			lua_pushnumber(L, rJoint->GetJointSpeed());
			lua_pushnumber(L, rJoint->GetMotorForce(1.0f));
			return 3;
		} else {
			logmsg("physics_getPrismaticJointValues called on non-prismatic joint");
		}
	}
	return 0;
}

static int lua_Game_physics_addRopeJoint(lua_State *L) {
    // param 1 is game
    // param 2 is gameobject1, 3 is index1
    // param 4 is gameobject2, 5 is index2
    // params 6,7 are anchorA X,Y
    // params 8,9 are anchorB X,Y
	// param 10 is maxLength
    // param 11 (optional) is collideConnected
     GameObject *objA = *(GameObject**)lua_touserdata(L, 2);
    S32 idxA = lua_tointeger(L, 3);
    GameObject *objB = *(GameObject**)lua_touserdata(L, 4);
    S32 idxB = lua_tointeger(L, 5);
    b2Body *myBodyA = getBody(objA, idxA);
    b2Body *myBodyB = getBody(objB, idxB);
    b2Vec2 worldAnchorOnBodyA = b2Vec2(lua_tonumber(L, 6), lua_tonumber(L, 7));
    b2Vec2 worldAnchorOnBodyB = b2Vec2(lua_tonumber(L, 8), lua_tonumber(L, 9));
    S32 jointID = nextJointID++;
    b2RopeJointDef jointDef;
    jointDef.bodyA = myBodyA;
    jointDef.bodyB = myBodyB;
    jointDef.localAnchorA = myBodyA->GetLocalPoint(worldAnchorOnBodyA);
    jointDef.localAnchorB = myBodyB->GetLocalPoint(worldAnchorOnBodyB);
    jointDef.maxLength = lua_tonumber(L, 10);
    if (lua_isboolean(L, 11)) {
        jointDef.collideConnected = lua_toboolean(L, 11);
    }
    b2Joint *joint = static_context->world->boxWorld->CreateJoint(&jointDef);
    joint->SetUserData((void*)jointID);
    static_context->world->boxJoints->put(jointID, joint);
    lua_pushinteger(L, jointID);
    return 1;
}

static int lua_Game_physics_addPulleyJoint(lua_State *L) {
    // param 1 is game
    // param 2 is gameobject1, 3 is index1
    // param 4 is gameobject2, 5 is index2
    // params 6,7 are groundAnchorA X,Y
    // params 8,9 are groundAnchorB X,Y
    // params 10,11 are anchorA X,Y
    // params 12,13 are anchorB X,Y
	// param 14 is ratio
    GameObject *objA = *(GameObject**)lua_touserdata(L, 2);
    S32 idxA = lua_tointeger(L, 3);
    GameObject *objB = *(GameObject**)lua_touserdata(L, 4);
    S32 idxB = lua_tointeger(L, 5);
    b2Body *myBodyA = getBody(objA, idxA);
    b2Body *myBodyB = getBody(objB, idxB);
    b2Vec2 groundAnchorA = b2Vec2(lua_tonumber(L, 6), lua_tonumber(L, 7));
    b2Vec2 groundAnchorB = b2Vec2(lua_tonumber(L, 8), lua_tonumber(L, 9));
    b2Vec2 worldAnchorOnBodyA = b2Vec2(lua_tonumber(L, 10), lua_tonumber(L, 11));
    b2Vec2 worldAnchorOnBodyB = b2Vec2(lua_tonumber(L, 12), lua_tonumber(L, 13));
    S32 jointID = nextJointID++;
    b2PulleyJointDef jointDef;
    jointDef.Initialize(myBodyA, myBodyB, groundAnchorA, groundAnchorB, worldAnchorOnBodyA, worldAnchorOnBodyB, lua_tonumber(L, 14));
    b2Joint *joint = static_context->world->boxWorld->CreateJoint(&jointDef);
    joint->SetUserData((void*)jointID);
    static_context->world->boxJoints->put(jointID, joint);
    lua_pushinteger(L, jointID);
    return 1;
}

static int lua_Game_physics_getPulleyJointValues(lua_State *L) {
	// param 1 is game
	// param 2 is joint ID
	// returns lengthA, lengthB
	S32 jointID = lua_tointeger(L, 2);
	b2Joint *joint = static_context->world->boxJoints->get(jointID);
	if (!joint) {
		char buf[255];
		sprintf(buf, "No pulley joint found for ID %d", jointID);
		logmsg(buf);
	} else {
		if (joint->GetType() == e_pulleyJoint) {
			b2PulleyJoint *rJoint = (b2PulleyJoint*) joint;
			lua_pushnumber(L, rJoint->GetLengthA());
			lua_pushnumber(L, rJoint->GetLengthB());
			return 3;
		} else {
			logmsg("physics_getPulleyJointValues called on non-pulley joint");
		}
	}
	return 0;
}

static int lua_Game_physics_addGearJoint(lua_State *L) {
    // param 1 is game
    // param 2 is gameobject1, 3 is index1
    // param 4 is gameobject2, 5 is index2
	// param 6 is jointIDA
	// param 7 is jointIDB
	// param 8 is ratio
    GameObject *objA = *(GameObject**)lua_touserdata(L, 2);
    S32 idxA = lua_tointeger(L, 3);
    GameObject *objB = *(GameObject**)lua_touserdata(L, 4);
    S32 idxB = lua_tointeger(L, 5);
    b2Body *myBodyA = getBody(objA, idxA);
    b2Body *myBodyB = getBody(objB, idxB);
    b2Joint *jointA = static_context->world->boxJoints->get(lua_tointeger(L, 6));
    b2Joint *jointB = static_context->world->boxJoints->get(lua_tointeger(L, 7));
    if (jointA && jointB) {
        S32 jointID = nextJointID++;
        b2GearJointDef jointDef;
        jointDef.bodyA = myBodyA;
        jointDef.bodyB = myBodyB;
        jointDef.joint1 = jointA;
        jointDef.joint2 = jointB;
        jointDef.ratio = lua_tonumber(L, 8);
        b2Joint *joint = static_context->world->boxWorld->CreateJoint(&jointDef);
        joint->SetUserData((void*)jointID);
        static_context->world->boxJoints->put(jointID, joint);
        lua_pushinteger(L, jointID);
        return 1;
    } else {
    	logmsg("Error - Joints not found to create gear");
    }
    return 0;
}

static int lua_Game_physics_addWheelJoint(lua_State *L) {
    // param 1 is game
    // param 2 is gameobject1, 3 is index1
    // param 4 is gameobject2, 5 is index2
    // params 6,7 are anchor X,Y
    // params 8,9 are axis X,Y
    // param 10 (optional) is collideConnected
    // param 11 (optional) is frequencyHz
    // param 12 (optional) is dampingRatio
    GameObject *objA = *(GameObject**)lua_touserdata(L, 2);
    S32 idxA = lua_tointeger(L, 3);
    GameObject *objB = *(GameObject**)lua_touserdata(L, 4);
    S32 idxB = lua_tointeger(L, 5);
    b2Body *myBodyA = getBody(objA, idxA);
    b2Body *myBodyB = getBody(objB, idxB);
    b2Vec2 worldAnchor = b2Vec2(lua_tonumber(L, 6), lua_tonumber(L, 7));
    b2Vec2 axis = b2Vec2(lua_tonumber(L, 8), lua_tonumber(L, 9));
    S32 jointID = nextJointID++;
    b2WheelJointDef jointDef;
    jointDef.Initialize(myBodyA, myBodyB, worldAnchor, axis);
    if (lua_isboolean(L, 10)) {
        jointDef.collideConnected = lua_toboolean(L, 10);
		if (lua_isnumber(L, 11)) {
            jointDef.frequencyHz = lua_tonumber(L, 11);
            if (lua_isnumber(L, 12)) {
                jointDef.dampingRatio = lua_tonumber(L, 12);
            }
        }
    }
    b2Joint *joint = static_context->world->boxWorld->CreateJoint(&jointDef);
    joint->SetUserData((void*)jointID);
    static_context->world->boxJoints->put(jointID, joint);
    lua_pushinteger(L, jointID);
    return 1;
}

static int lua_Game_physics_setWheelJointMotor(lua_State *L) {
    // param 1 is game
	// param 2 is jointID
	// param 3 is motor enabled
	// param 4 is motor speed (opt)
	// param 5 is max motor torque (opt)
	S32 jointID = lua_tointeger(L, 2);
	if (static_context->world->boxJoints) {
		b2Joint *joint = static_context->world->boxJoints->get(jointID);
		if (!joint) {
			char buf[255];
			sprintf(buf, "No wheel joint found for ID %d", jointID);
			logmsg(buf);
		} else {
			if (joint->GetType() == e_wheelJoint) {
				b2WheelJoint *rJoint = (b2WheelJoint*) joint;
				rJoint->EnableMotor(lua_toboolean(L, 3));
				if (lua_isnumber(L, 4)) {
					rJoint->SetMotorSpeed(lua_tonumber(L, 4));
				}
				if (lua_isnumber(L, 5)) {
					rJoint->SetMaxMotorTorque(lua_tonumber(L, 5));
				}
			} else {
				logmsg("physics_setWheelJointMotor called on non-wheel joint");
			}
		}
	}
	return 0;
}

static int lua_Game_physics_getWheelJointValues(lua_State *L) {
	// param 1 is game
	// param 2 is joint ID
	// returns translation, speed and torque
	S32 jointID = lua_tointeger(L, 2);
	b2Joint *joint = static_context->world->boxJoints->get(jointID);
	if (!joint) {
		char buf[255];
		sprintf(buf, "No wheel joint found for ID %d", jointID);
		logmsg(buf);
	} else {
		if (joint->GetType() == e_wheelJoint) {
			b2WheelJoint *rJoint = (b2WheelJoint*) joint;
			lua_pushnumber(L, rJoint->GetJointTranslation());
			lua_pushnumber(L, rJoint->GetJointSpeed());
			lua_pushnumber(L, rJoint->GetMotorTorque(1.0f));
			return 3;
		} else {
			logmsg("physics_getWheelJointValues called on non-wheel joint");
		}
	}
	return 0;
}

static int lua_Game_physics_addWeldJoint(lua_State *L) {
    // param 1 is game
    // param 2 is gameobject1, 3 is index1
    // param 4 is gameobject2, 5 is index2
    // params 6,7 are anchorA X,Y
    // param 8 (optional) is collideConnected
    // param 9 (optional) is frequencyHz
    // param 10 (optional) is dampingRatio
    GameObject *objA = *(GameObject**)lua_touserdata(L, 2);
    S32 idxA = lua_tointeger(L, 3);
    GameObject *objB = *(GameObject**)lua_touserdata(L, 4);
    S32 idxB = lua_tointeger(L, 5);
    b2Body *myBodyA = getBody(objA, idxA);
    b2Body *myBodyB = getBody(objB, idxB);
    b2Vec2 worldAnchorOnBodyA = b2Vec2(lua_tonumber(L, 6), lua_tonumber(L, 7));
    S32 jointID = nextJointID++;
    b2WeldJointDef jointDef;
    jointDef.Initialize(myBodyA, myBodyB, worldAnchorOnBodyA);
    if (lua_isboolean(L, 8)) {
        jointDef.collideConnected = lua_toboolean(L, 8);
        if (lua_isnumber(L, 9)) {
            jointDef.frequencyHz = lua_tonumber(L, 9);
            if (lua_isnumber(L, 10)) {
                jointDef.dampingRatio = lua_tonumber(L, 10);
            }
        }
    }
    b2Joint *joint = static_context->world->boxWorld->CreateJoint(&jointDef);
    joint->SetUserData((void*)jointID);
    static_context->world->boxJoints->put(jointID, joint);
    lua_pushinteger(L, jointID);
    return 1;
}

static int lua_Game_physics_addFrictionJoint(lua_State *L) {
    // param 1 is game
    // param 2 is gameobject1, 3 is index1
    // param 4 is gameobject2, 5 is index2
    // params 6,7 are anchorA X,Y
    // param 8 is maxForce
    // param 9 is maxTorque
    GameObject *objA = *(GameObject**)lua_touserdata(L, 2);
    S32 idxA = lua_tointeger(L, 3);
    GameObject *objB = *(GameObject**)lua_touserdata(L, 4);
    S32 idxB = lua_tointeger(L, 5);
    b2Body *myBodyA = getBody(objA, idxA);
    b2Body *myBodyB = getBody(objB, idxB);
    b2Vec2 worldAnchorOnBodyA = b2Vec2(lua_tonumber(L, 6), lua_tonumber(L, 7));
    S32 jointID = nextJointID++;
    b2FrictionJointDef jointDef;
    jointDef.Initialize(myBodyA, myBodyB, worldAnchorOnBodyA);
    jointDef.maxForce = lua_tonumber(L, 8);
    jointDef.maxTorque = lua_tonumber(L, 9);
    b2Joint *joint = static_context->world->boxWorld->CreateJoint(&jointDef);
    joint->SetUserData((void*)jointID);
    static_context->world->boxJoints->put(jointID, joint);
    lua_pushinteger(L, jointID);
    return 1;
}

static int lua_Game_physics_removeJoint(lua_State *L) {
    // param 1 is game
    // param 2 is joint ID
    S32 jointID = lua_tointeger(L, 2);
    b2Joint *joint = static_context->world->boxJoints->get(jointID);
    if (joint) {
        // no further operation is needed because the destroy operation will trigger a callback removing it from the hashtable
        static_context->world->boxWorld->DestroyJoint(joint);
    }
    return 0;
}

static int lua_Game_physics_getJointAnchorPoints(lua_State *L) {
    // param 1 is game
    // param 2 is joint ID
    S32 jointID = lua_tointeger(L, 2);
    b2Joint *joint = static_context->world->boxJoints->get(jointID);
    if (joint) {
        lua_pushnumber(L, joint->GetAnchorA().x);
        lua_pushnumber(L, joint->GetAnchorA().y);
        lua_pushnumber(L, joint->GetAnchorB().x);
        lua_pushnumber(L, joint->GetAnchorB().y);
        return 4;
    }
    return 0;
}


#endif

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

// ---------------- callbacks for box2d -----------------

#ifdef BATTERYTECH_INCLUDE_BOX2D
	bool LuaBinderQueryCallback::ReportFixture(b2Fixture* fixture) {
		b2Body* body = fixture->GetBody();
		PhysicsBodyObject *o1 = (PhysicsBodyObject*) body->GetUserData();
		if (o1->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
			GameObject *go = (GameObject*) o1;
			lua_rawgeti(L, LUA_REGISTRYINDEX, go->luaBinder->luaRef);
            lua_pushinteger(L, (size_t)fixture->GetUserData());
			returnCount+=2;
		}
		// Return true to continue the query.
		return true;
	}
#endif
