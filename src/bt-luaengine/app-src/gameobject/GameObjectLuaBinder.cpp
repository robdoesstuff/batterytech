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

#include "GameObjectLuaBinder.h"
#include "GameObject.h"
#include "../GameContext.h"
#include <batterytech/Logger.h>
#include "../World.h"
#include <batterytech/util/strx.h>
#include <math.h>
#include <batterytech/render/AssimpAnimator.h>
#include <batterytech/render/GLResourceManager.h>
#include <batterytech/render/GLAssimpBinding.h>

#ifdef BATTERYTECH_INCLUDE_BOX2D
#include <bt-box2d/Collision/Shapes/b2Shape.h>
#include <bt-box2d/Collision/Shapes/b2PolygonShape.h>
#include <bt-box2d/Collision/Shapes/b2CircleShape.h>
#include <bt-box2d/Collision/Shapes/b2ChainShape.h>
#endif

#define LUA_GAME_OBJECT "GameObject"
#define LUA_GAME_OBJECT_MT "GameObjectMetaTable"

static GameContext *static_context;

// c metafunctions available to lua
static int lua_GameObject_gc (lua_State *L);
static int lua_GameObject_tostring (lua_State *L);

// c functions available to lua
static int lua_GameObject_createInstance(lua_State *L);
static int lua_GameObject_replaceMetatable(lua_State *L);
static int lua_GameObject_init(lua_State *L);
static int lua_GameObject_deactivate(lua_State *L); // once an object is deactivated, no further calls may be made on it or its luabinder or crashes will occur
// 3D Animation functions
static int lua_GameObject_anim_allocAnimations(lua_State *L); // allocates n animations
static int lua_GameObject_anim_initDynamic(lua_State *L); // initializes animation n for dynamic model x
static int lua_GameObject_anim_interpolate(lua_State *L); // interpolates animation n at time x

#ifdef BATTERYTECH_INCLUDE_BULLET
// 3D Physics functions
static int lua_GameObject_setModelScale(lua_State *L);
static int lua_GameObject_getModelScale(lua_State *L);
static int lua_GameObject_setPosition(lua_State *L);
static int lua_GameObject_getPosition(lua_State *L);
static int lua_GameObject_setAllOrientationYPR(lua_State *L);
static int lua_GameObject_setOrientationYPR(lua_State *L);
static int lua_GameObject_getOrientationYPR(lua_State *L);
static int lua_GameObject_applyForce(lua_State *L); // applies toward primary btBody
static int lua_GameObject_applyImpulse(lua_State *L); // applies toward primary btBody
static int lua_GameObject_setLinearVelocity(lua_State *L);
static int lua_GameObject_getLinearVelocity(lua_State *L);
static int lua_GameObject_getOpenGLMatrix(lua_State *L);
static int lua_GameObject_setMass(lua_State *L);
// Deprecated - should remove and move to lua
static int lua_GameObject_moveTowards(lua_State *L);
static int lua_GameObject_getPathLength(lua_State *L);
static int lua_GameObject_getPathPoint(lua_State *L);
static int lua_GameObject_addPathPoint(lua_State *L);
static int lua_GameObject_clearPath(lua_State *L);
static int lua_GameObject_isSelected(lua_State *L);
static int lua_GameObject_setSelected(lua_State *L);
static int lua_GameObject_setPathingParameters(lua_State *L); // param: accel, speedClamp, rotationSpeed (in degrees per second)

// Bullet bindings
static int lua_GameObject_physics_allocConfigs(lua_State *L); // param: number of physics model configurations, defaults to 1 if not called
static int lua_GameObject_physics_createBox(lua_State *L);
static int lua_GameObject_physics_createCylinder(lua_State *L);
static int lua_GameObject_physics_createSphere(lua_State *L);
static int lua_GameObject_physics_setMass(lua_State *L);
static int lua_GameObject_physics_setPosition(lua_State *L);
static int lua_GameObject_physics_setFriction(lua_State *L);
static int lua_GameObject_physics_setAngularFactor(lua_State *L);
static int lua_GameObject_physics_setActivationState(lua_State *L);
static int lua_GameObject_physics_setCollisionFilter(lua_State *L);
static int lua_GameObject_physics_setCollisionFlags(lua_State *L);
static int lua_GameObject_physics_setGravity(lua_State *L);
static int lua_GameObject_physics_setDamping(lua_State *L);
static int lua_GameObject_physics_setGhost(lua_State *L);
static int lua_GameObject_physics_addFixedHingeConstraint(lua_State *L);
static int lua_GameObject_physics_addHingeConstraint(lua_State *L); // param: body0, body1, offsetA, offsetB, axisA, axisB, disableCollision - returns index of constraint
static int lua_GameObject_physics_addP2PConstraint(lua_State *L); // param: body0, body1, offsetA, offsetB, axisA, axisB, disableCollision - returns index of constraint
static int lua_GameObject_physics_setConstraintLimits(lua_State *L); // param: index, low, high
static int lua_GameObject_physics_enableConstraintMotor(lua_State *L);
static int lua_GameObject_physics_enableConstraintMotorTarget(lua_State *L);
#endif

#ifdef BATTERYTECH_INCLUDE_BOX2D
// Box2D bindings
// b2Body
static int lua_GameObject_physics_createBody(lua_State *L);
static int lua_GameObject_physics_removeBody(lua_State *L);
static int lua_GameObject_physics_setBodyTransform(lua_State *L);
static int lua_GameObject_physics_setBodyAngularVelocity(lua_State *L);
static int lua_GameObject_physics_setBodyLinearVelocity(lua_State *L);
static int lua_GameObject_physics_setBodyAngularDamping(lua_State *L);
static int lua_GameObject_physics_setBodyLinearDamping(lua_State *L);
static int lua_GameObject_physics_setBodyAllowSleep(lua_State *L);
static int lua_GameObject_physics_setBodyAwake(lua_State *L);
static int lua_GameObject_physics_setBodyFixedRotation(lua_State *L);
static int lua_GameObject_physics_setBodyBullet(lua_State *L);
static int lua_GameObject_physics_setBodyActive(lua_State *L);
static int lua_GameObject_physics_setBodyType(lua_State *L);
static int lua_GameObject_physics_setBodyGravityScale(lua_State *L);
static int lua_GameObject_physics_getBodyTransform(lua_State *L);
static int lua_GameObject_physics_getBodyAngularVelocity(lua_State *L);
static int lua_GameObject_physics_getBodyLinearVelocity(lua_State *L);
static int lua_GameObject_physics_getBodyWorldPoint(lua_State *L);
static int lua_GameObject_physics_getBodyWorldPoints(lua_State *L);
static int lua_GameObject_physics_getBodyLocalPoint(lua_State *L);
static int lua_GameObject_physics_getBodyLocalCenter(lua_State *L);
static int lua_GameObject_physics_applyForce(lua_State *L);
static int lua_GameObject_physics_applyTorque(lua_State *L);
static int lua_GameObject_physics_applyLinearImpulse(lua_State *L);
static int lua_GameObject_physics_applyAngularImpulse(lua_State *L);
// b2Shape and b2Fixture
static int lua_GameObject_physics_createPolygonFixture(lua_State *L);
static int lua_GameObject_physics_createCircleFixture(lua_State *L);
static int lua_GameObject_physics_createChainFixture(lua_State *L);
static int lua_GameObject_physics_removeFixture(lua_State *L);
static int lua_GameObject_physics_setFixtureDensity(lua_State *L);
static int lua_GameObject_physics_setFixtureFriction(lua_State *L);
static int lua_GameObject_physics_setFixtureRestitution(lua_State *L);
static int lua_GameObject_physics_setFixtureIsSensor(lua_State *L);
static int lua_GameObject_physics_setFixtureFilter(lua_State *L);
static int lua_GameObject_physics_fixtureTestPoint(lua_State *L);
static int lua_GameObject_physics_getFixtureBodyId(lua_State *L);
// collision query
static int lua_GameObject_queryPhysicsContacts(lua_State *L);
// collision callbacks
static int lua_GameObject_setPhysicsCallbackDetail(lua_State *L);
static int lua_GameObject_countPhysicsContacts(lua_State *L);
static int lua_GameObject_getPhysicsContact(lua_State *L);

#endif

static const luaL_reg lua_methods[] = {
	{ "createInstance", lua_GameObject_createInstance },
	{ "replaceMetatable", lua_GameObject_replaceMetatable },
	{ "cInit", lua_GameObject_init },
	{ "cDeactivate", lua_GameObject_deactivate },
	{ "anim_allocAnimations", lua_GameObject_anim_allocAnimations },
	{ "anim_initDynamic", lua_GameObject_anim_initDynamic },
	{ "anim_interpolate", lua_GameObject_anim_interpolate },
#ifdef BATTERYTECH_INCLUDE_BULLET
	{ "setModelScale", lua_GameObject_setModelScale },
	{ "getModelScale", lua_GameObject_getModelScale },
	{ "setPosition", lua_GameObject_setPosition },
	{ "getPosition", lua_GameObject_getPosition },
	{ "setAllOrientationYPR", lua_GameObject_setAllOrientationYPR },
	{ "setOrientationYPR", lua_GameObject_setOrientationYPR },
	{ "getOrientationYPR", lua_GameObject_getOrientationYPR },
	{ "moveTowards", lua_GameObject_moveTowards },
	{ "applyForce", lua_GameObject_applyForce },
	{ "applyImpulse", lua_GameObject_applyImpulse },
	{ "getPathLength", lua_GameObject_getPathLength },
	{ "getPathPoint", lua_GameObject_getPathPoint },
	{ "addPathPoint", lua_GameObject_addPathPoint },
	{ "clearPath", lua_GameObject_clearPath },
	{ "isSelected", lua_GameObject_isSelected },
	{ "setSelected", lua_GameObject_setSelected },
	{ "setPathingParameters", lua_GameObject_setPathingParameters },
	{ "setLinearVelocity", lua_GameObject_setLinearVelocity },
	{ "getLinearVelocity", lua_GameObject_getLinearVelocity },
	{ "getOpenGLMatrix", lua_GameObject_getOpenGLMatrix },
	{ "setMass", lua_GameObject_setMass },
	{ "physics_allocConfigs", lua_GameObject_physics_allocConfigs },
	{ "physics_createBox", lua_GameObject_physics_createBox },
	{ "physics_createCylinder", lua_GameObject_physics_createCylinder },
	{ "physics_createSphere", lua_GameObject_physics_createSphere },
	{ "physics_setMass", lua_GameObject_physics_setMass },
	{ "physics_setPosition", lua_GameObject_physics_setPosition },
	{ "physics_setFriction", lua_GameObject_physics_setFriction },
	{ "physics_setAngularFactor", lua_GameObject_physics_setAngularFactor },
	{ "physics_setActivationState", lua_GameObject_physics_setActivationState },
	{ "physics_setCollisionFilter", lua_GameObject_physics_setCollisionFilter },
	{ "physics_setCollisionFlags", lua_GameObject_physics_setCollisionFlags },
	{ "physics_setGravity", lua_GameObject_physics_setGravity },
	{ "physics_setDamping", lua_GameObject_physics_setDamping },
	{ "physics_setGhost", lua_GameObject_physics_setGhost },
	{ "physics_addFixedHingeConstraint", lua_GameObject_physics_addFixedHingeConstraint },
	{ "physics_addHingeConstraint", lua_GameObject_physics_addHingeConstraint },
	{ "physics_addP2PConstraint", lua_GameObject_physics_addP2PConstraint },
	{ "physics_setConstraintLimits", lua_GameObject_physics_setConstraintLimits },
	{ "physics_enableConstraintMotor", lua_GameObject_physics_enableConstraintMotor },
	{ "physics_enableConstraintMotorTarget", lua_GameObject_physics_enableConstraintMotorTarget },
#endif
#ifdef BATTERYTECH_INCLUDE_BOX2D
// Box2D bindings
	{ "physics_createBody", lua_GameObject_physics_createBody },
	{ "physics_removeBody", lua_GameObject_physics_removeBody },
    { "physics_setBodyTransform", lua_GameObject_physics_setBodyTransform },
    { "physics_setBodyAngularVelocity", lua_GameObject_physics_setBodyAngularVelocity },
    { "physics_setBodyLinearVelocity", lua_GameObject_physics_setBodyLinearVelocity },
    { "physics_setBodyAngularDamping", lua_GameObject_physics_setBodyAngularDamping },
    { "physics_setBodyLinearDamping", lua_GameObject_physics_setBodyLinearDamping },
    { "physics_SetBodyAllowSleep", lua_GameObject_physics_setBodyAllowSleep },
    { "physics_setBodyAwake", lua_GameObject_physics_setBodyAwake },
    { "physics_setBodyFixedRotation", lua_GameObject_physics_setBodyFixedRotation },
    { "physics_setBodyBullet", lua_GameObject_physics_setBodyBullet },
    { "physics_setBodyActive", lua_GameObject_physics_setBodyActive },
    { "physics_setBodyType", lua_GameObject_physics_setBodyType },
    { "physics_setBodyGravityScale", lua_GameObject_physics_setBodyGravityScale },
    { "physics_getBodyTransform", lua_GameObject_physics_getBodyTransform },
    { "physics_getBodyAngularVelocity", lua_GameObject_physics_getBodyAngularVelocity },
    { "physics_getBodyLinearVelocity", lua_GameObject_physics_getBodyLinearVelocity },
    { "physics_getBodyWorldPoint", lua_GameObject_physics_getBodyWorldPoint },
    { "physics_getBodyWorldPoints", lua_GameObject_physics_getBodyWorldPoints },
    { "physics_getBodyLocalPoint", lua_GameObject_physics_getBodyLocalPoint },
    { "physics_getBodyLocalCenter", lua_GameObject_physics_getBodyLocalCenter },
	{ "physics_applyForce", lua_GameObject_physics_applyForce },
	{ "physics_applyTorque", lua_GameObject_physics_applyTorque },
	{ "physics_applyLinearImpulse", lua_GameObject_physics_applyLinearImpulse },
	{ "physics_applyAngularImpulse", lua_GameObject_physics_applyAngularImpulse },
	{ "physics_createPolygonFixture", lua_GameObject_physics_createPolygonFixture },
	{ "physics_createCircleFixture", lua_GameObject_physics_createCircleFixture },
	{ "physics_createChainFixture", lua_GameObject_physics_createChainFixture },
	{ "physics_removeFixture", lua_GameObject_physics_removeFixture },
    { "physics_setFixtureDensity", lua_GameObject_physics_setFixtureDensity },
    { "physics_setFixtureFriction", lua_GameObject_physics_setFixtureFriction },
    { "physics_setFixtureRestitution", lua_GameObject_physics_setFixtureRestitution },
    { "physics_setFixtureIsSensor", lua_GameObject_physics_setFixtureIsSensor },
    { "physics_setFixtureFilter", lua_GameObject_physics_setFixtureFilter },
    { "physics_fixtureTestPoint", lua_GameObject_physics_fixtureTestPoint },
    { "physics_getFixtureBodyId", lua_GameObject_physics_getFixtureBodyId },
    { "queryPhysicsContacts", lua_GameObject_queryPhysicsContacts },
    { "setPhysicsCallbackDetail", lua_GameObject_setPhysicsCallbackDetail },
    { "countPhysicsContacts", lua_GameObject_countPhysicsContacts },
    { "getPhysicsContact", lua_GameObject_getPhysicsContact },
#endif

	{ 0, 0 } };

static const luaL_reg metatable[] = {
	{"__gc",       lua_GameObject_gc},
	{"__tostring", lua_GameObject_tostring},
	{0, 0}
};

GameObjectLuaBinder::GameObjectLuaBinder(lua_State *L, GameObject *gameObject, GameContext *context) : LuaBinder(L) {
	this->gameObject = gameObject;
	this->context = context;
	luaRef = 0;
}

GameObjectLuaBinder::~GameObjectLuaBinder() {
	// be very careful with this on destruction...
	if (luaRef && L && context->world && context->world->luaState) {
		luaL_unref(L, LUA_REGISTRYINDEX, luaRef);
	}
	gameObject = NULL;
	luaRef = 0;
	L = NULL;
}

void GameObjectLuaBinder::luaRegister(lua_State *L, GameContext *context) {
	static_context = context;
	luaL_openlib(L, LUA_GAME_OBJECT, lua_methods, 0);  /* create methods table, add it to the globals */
	luaL_newmetatable(L, LUA_GAME_OBJECT_MT);        /* create metatable for Game,  add it to the Lua registry */
	luaL_openlib(L, 0, metatable, 0);  /* fill metatable */
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);               /* dup methods table*/
	lua_rawset(L, -3);                  /* metatable.__index = Game methods */
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, -2);               /* dup metatable*/
	lua_rawset(L, -3);                  /* hide metatable:  metatable.__metatable = metatable methods */
	lua_settop(L, -3);					// put the stack back to where we started
}

// unused - clean up
void GameObjectLuaBinder::init() {
	if (pushInstanceFunction("init")) {
		callFunction("GameObject:init", TRUE);
	}
}

void GameObjectLuaBinder::update() {
	if (pushInstanceFunction("update")) {
		callFunctionVA("GameObject:update", TRUE, "d>", context->tickDelta);
	}
}

void GameObjectLuaBinder::onCollisionStarted(GameObject *other, F32 force, F32 approachVelocity) {
	S32 otherRef = other->luaBinder->luaRef;
	if (pushInstanceFunction("onCollisionStarted")) {
		callFunctionVA("GameObject:onCollisionStarted", TRUE, "rdd>", otherRef, force, approachVelocity);
	}
}

void GameObjectLuaBinder::onCollisionUpdated(GameObject *other, F32 force, F32 approachVelocity) {
	S32 otherRef = other->luaBinder->luaRef;
	if (pushInstanceFunction("onCollisionUpdated")) {
		callFunctionVA("GameObject:onCollisionUpdated", TRUE, "rdd>", otherRef, force, approachVelocity);
	}
}

void GameObjectLuaBinder::onCollisionEnded(GameObject *other) {
	S32 otherRef = other->luaBinder->luaRef;
	if (pushInstanceFunction("onCollisionEnded")) {
		callFunctionVA("GameObject:onCollisionEnded", TRUE, "r>", otherRef);
	}
}

void GameObjectLuaBinder::onLevelCollision() {
	if (pushInstanceFunction("onLevelCollision")) {
		callFunctionVA("GameObject:onLevelCollision", TRUE, ">");
	}
}

void GameObjectLuaBinder::onLevelCollisionEnded() {
	if (pushInstanceFunction("onLevelCollisionEnded")) {
		callFunctionVA("GameObject:onLevelCollisionEnded", TRUE, ">");
	}
}
BOOL32 GameObjectLuaBinder::pushInstanceFunction(const char *name) {
	lua_rawgeti( L, LUA_REGISTRYINDEX, luaRef ); // push the gameRef we made onto the stack - userdata at -1
	lua_pushstring( L, name ); // update, metatable, userdata
	lua_gettable(L, -2); // get the function from the __index (don't use rawget since it doesn't use index)
	lua_remove( L, -2 ); //we have the function now so no longer need the name of it.  // function, table, userdata
	if( lua_isfunction( L, -1 ) == 1 ) {
		lua_rawgeti( L, LUA_REGISTRYINDEX, luaRef ); // use "self" as parameter
		return TRUE;
	}
	lua_settop(L, -2);
	return FALSE;
}

char* GameObjectLuaBinder::getString(const char* name) {
	char *string;
	lua_rawgeti( L, LUA_REGISTRYINDEX, luaRef ); // push the gameRef we made onto the stack - userdata at -1
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

BOOL32 GameObjectLuaBinder::getBool(const char* name) {
	BOOL32 result = FALSE;
	lua_rawgeti( L, LUA_REGISTRYINDEX, luaRef ); // push the gameRef we made onto the stack - userdata at -1
	lua_pushstring( L, name ); // update, metatable, userdata
	lua_gettable(L, -2); // get the function from the __index (don't use rawget since it doesn't use index)
	if (lua_isboolean(L, -1)) {
		result = lua_toboolean(L, -1);
	}
	lua_settop(L, -3); // put the stack back to where we started
	return result;
}

S32 GameObjectLuaBinder::getInt(const char* name) {
	S32 result = 0;
	lua_rawgeti( L, LUA_REGISTRYINDEX, luaRef ); // push the gameRef we made onto the stack - userdata at -1
	lua_pushstring( L, name ); // update, metatable, userdata
	lua_gettable(L, -2); // get the function from the __index (don't use rawget since it doesn't use index)
	if (lua_isnumber(L, -1)) {
		result = lua_tointeger(L, -1);
	}
	lua_settop(L, -3); // put the stack back to where we started
	return result;
}

F32 GameObjectLuaBinder::getFloat(const char* name) {
	F32 result = 0;
	lua_rawgeti( L, LUA_REGISTRYINDEX, luaRef ); // push the gameRef we made onto the stack - userdata at -1
	lua_pushstring( L, name ); // update, metatable, userdata
	lua_gettable(L, -2); // get the function from the __index (don't use rawget since it doesn't use index)
	if (lua_isnumber(L, -1)) {
		result = (F32)lua_tonumber(L, -1);
	}
	lua_settop(L, -3); // put the stack back to where we started
	return result;
}

ManagedArray<GameObjectProperty>* GameObjectLuaBinder::getProperties() {
	lua_rawgeti( L, LUA_REGISTRYINDEX, luaRef ); // push the gameRef we made onto the stack - userdata at -1
	lua_pushstring( L, "properties" ); // update, metatable, userdata
	lua_gettable(L, -2); // get the editor objects table from the __index (don't use rawget since it doesn't use index)
	char buf[255];
	ManagedArray<GameObjectProperty> *props = NULL;
	if (lua_istable(L, -1)) {
		logmsg("Properties table found");
		S32 count = 0;
		for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
			count++;
		}
		sprintf(buf, "%d Properties Counted", count);
		logmsg(buf);
		props = new ManagedArray<GameObjectProperty>(count);
		S32 i = 0;
		for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
			if (lua_isstring(L, -2)) {
				const char *key = lua_tostring(L, -2);
				char *value = NULL;
				GameObjectProperty::PropertyType propType;
				if (lua_isnumber(L, -1)) {
					F32 f = lua_tonumber(L, -1);
					if (floor(f) == f) {
						// is integer
						sprintf(buf, "%d", (S32)f);
						propType = GameObjectProperty::PROP_TYPE_INTEGER;
					} else {
						sprintf(buf, "%f", f);
						propType = GameObjectProperty::PROP_TYPE_NUMBER;
					}
					value = strDuplicate(buf);
				} else if (lua_isstring(L, -1)) {
					value = strDuplicate(lua_tostring(L, -1));
					propType = GameObjectProperty::PROP_TYPE_STRING;
				} else if (lua_isboolean(L, -1)) {
					BOOL32 b = lua_toboolean(L, -1);
					value = b ? strDuplicate("true") : strDuplicate("false");
					propType = GameObjectProperty::PROP_TYPE_BOOLEAN;
				}
				sprintf(buf, "Property %s=%s", key, value);
				logmsg(buf);
				GameObjectProperty *prop = new GameObjectProperty;
				prop->name = strDuplicate(key);
				prop->stringValue = value;
				prop->pType = propType;
				props->add(prop);
				i++;
			}
		}
	} else {
		// no objects
	}
	lua_settop(L, -3); // put the stack back to where we started
	return props;
}

void GameObjectLuaBinder::setPropertyS32(const char* key, S32 value) {
	if (pushInstanceFunction("setProperty")) {
		callFunctionVA("GameObject:setProperty", TRUE, "si>", key, value);
	}
}

void GameObjectLuaBinder::setPropertyF32(const char* key, F32 value) {
	if (pushInstanceFunction("setProperty")) {
		callFunctionVA("GameObject:setProperty", TRUE, "sd>", key, value);
	}
}

void GameObjectLuaBinder::setPropertyBool32(const char* key, BOOL32 value) {
	if (pushInstanceFunction("setProperty")) {
		callFunctionVA("GameObject:setProperty", TRUE, "sb>", key, value);
	}
}

void GameObjectLuaBinder::setPropertyString(const char* key, const char* value) {
	if (pushInstanceFunction("setProperty")) {
		callFunctionVA("GameObject:setProperty", TRUE, "ss>", key, value);
	}
}

static int lua_GameObject_init(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	o->init();
	return 0;
}

static int lua_GameObject_deactivate(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	o->isActive = FALSE;
	return 0;
}

static int lua_GameObject_createInstance(lua_State *L) {
	// logmsg("lua_GameObject_createInstance");
	// LuaBinder::stackDump(L);
	// We use a double pointer to support GameObject pooling
	GameObject **lGamePtr = (GameObject **) lua_newuserdata(L, sizeof(GameObject*));
	GameObject *gameObj = new GameObject(static_context); // instantiates its own binder
	static_context->world->gameObjects->add(gameObj);
	*lGamePtr = gameObj; // assign the pointer to our already-established gameobject userdata
	lua_pushvalue( L, -1 ); // push game userdata again on the stack
	gameObj->luaBinder->luaRef = luaL_ref( L, LUA_REGISTRYINDEX ); // make a ref number for the lua object
	// LuaBinder::stackDump(L);
	if (lua_type(L, -2) == LUA_TTABLE) {
		// table passed in as metatable (inheritence)
		// logmsg("Using user-defined table");
	} else {
		luaL_getmetatable(L, LUA_GAME_OBJECT_MT); // use the base GameObject metatable
	}
	lua_setmetatable(L, -2); // set the metatable to the gameobject instance
	return 1;
}

static int lua_GameObject_replaceMetatable(lua_State *L) {
	lua_setmetatable(L, -2); // -1 should be self, -2 should be metatable
	return 0;
}

// allocates n animations
static int lua_GameObject_anim_allocAnimations(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 count = lua_tonumber(L, 2);
	o->animators = new ManagedArray<AssimpAnimator>(count);
	for (S32 i = 0; i < count; i++) {
		o->animators->add(new AssimpAnimator());
	}
	return 0;
}

// initializes animation n for dynamic model x mesh y
static int lua_GameObject_anim_initDynamic(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	// find the dynamic model, give aiScene to animator
	S32 idx = lua_tointeger(L, 2);
	const char *assetName = lua_tostring(L, 3);
	const char *meshName = NULL;
	if (lua_isstring(L, 4)) {
		meshName = lua_tostring(L, 4);
	}
	GLAssimpBinding *binding = static_context->glResourceManager->getAssimp(assetName);
	if (binding) {
        if (!binding->scene) {
            char buf[1024];
            sprintf(buf, "Unable to initialize animator for asset %s, asset added but not yet loaded", assetName);
            logmsg(buf);
        } else {
            o->animators->array[idx]->init(binding, meshName);
        }
	} else {
		logmsg("Assimp Binding not found in anim_initDynamic");
	}
	return 0;
}

// interpolates animation n at time x
static int lua_GameObject_anim_interpolate(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 idx = lua_tointeger(L, 2);
	F32 time = lua_tonumber(L, 3);
	o->animators->array[idx]->interpolate(time);
	return 0;
}

#ifdef BATTERYTECH_INCLUDE_BULLET
static int lua_GameObject_setModelScale(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	F32 x = lua_tonumber(L, 2);
	F32 y = lua_tonumber(L, 3);
	F32 z = lua_tonumber(L, 4);
	o->setModelScale(Vector3f(x,y,z));

	if (o->btBody) {
		o->btBody->activate(false);
	}

	return 0;
}

static int lua_GameObject_getModelScale(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	Vector3f scale = o->getModelScale();
	lua_pushnumber(L, scale.x);
	lua_pushnumber(L, scale.y);
	lua_pushnumber(L, scale.z);
	return 3;
}

static int lua_GameObject_setPosition(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	F32 x = lua_tonumber(L, 2);
	F32 y = lua_tonumber(L, 3);
	F32 z = lua_tonumber(L, 4);
	o->setPosition(Vector3f(x,y,z));

	if (o->btBody) {
		o->btBody->activate(false);
	}

	return 0;
}

static int lua_GameObject_getPosition(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	Vector3f pos = o->getPosition();
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	lua_pushnumber(L, pos.z);
	return 3;
}

static int lua_GameObject_setAllOrientationYPR(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	F32 y = lua_tonumber(L, 2);
	F32 p = lua_tonumber(L, 3);
	F32 r = lua_tonumber(L, 4);
	o->setAllOrientationYPR(Vector3f(y,p,r));
	return 0;
}

static int lua_GameObject_setOrientationYPR(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 idx = lua_tointeger(L, 2);
	F32 y = lua_tonumber(L, 3);
	F32 p = lua_tonumber(L, 4);
	F32 r = lua_tonumber(L, 5);
	o->setOrientationYPR(idx, Vector3f(y,p,r));
	return 0;
}
static int lua_GameObject_getOrientationYPR(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 idx = 0;
	if (lua_isnumber(L, 2)) {
		idx = lua_tointeger(L, 2);
	}
	Vector3f ypr = o->getOrientationYPR(idx);
	lua_pushnumber(L, ypr.x);
	lua_pushnumber(L, ypr.y);
	lua_pushnumber(L, ypr.z);
	return 0;
}

static int lua_GameObject_moveTowards(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	F32 x = lua_tonumber(L, 2);
	F32 y = lua_tonumber(L, 3);
	F32 z = lua_tonumber(L, 4);
	//o->path[0] = GameObject::PathNode(Vector3f(x,y,z), NULL);
	//o->pathLength = 1;
	return 0;
}

static int lua_GameObject_applyForce(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);

	if (o->btBody) {
		F32 x = lua_tonumber(L, 2);
		F32 y = lua_tonumber(L, 3);
		F32 z = lua_tonumber(L, 4);
		o->btBody->applyCentralForce(btVector3(x,y,z));
		o->btBody->activate(false);
	}

	return 0;
}

static int lua_GameObject_applyImpulse(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);

	if (o->btBody) {
		F32 x = lua_tonumber(L, 2);
		F32 y = lua_tonumber(L, 3);
		F32 z = lua_tonumber(L, 4);
		o->btBody->applyCentralImpulse(btVector3(x,y,z));
		o->btBody->activate(false);
	}

	return 0;
}

static int lua_GameObject_getPathLength(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	//lua_pushinteger(L, o->pathLength);
	return 1;
}

static int lua_GameObject_getPathPoint(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);

	S32 idx = lua_tointeger(L, 2);
	if (idx < GAMEOBJECT_MAX_PATH) {
		Vector3f p = o->path[idx].pos;
		lua_pushnumber(L, p.x);
		lua_pushnumber(L, p.y);
		lua_pushnumber(L, p.z);
		return 3;
	} else {
		return 0;
	}

	return 0;
}

static int lua_GameObject_addPathPoint(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);

	if (o->pathLength < GAMEOBJECT_MAX_PATH - 1) {
		F32 x = lua_tonumber(L, 2);
		F32 y = lua_tonumber(L, 3);
		F32 z = lua_tonumber(L, 4);
		GameObject *obj = NULL;
		if (lua_isuserdata(L, 5)) {
			obj = *(GameObject**)lua_touserdata(L, 5);
		}
		o->path[o->pathLength++] = GameObject::PathNode(Vector3f(x,y,z), obj);
	}

	return 0;
}

static int lua_GameObject_clearPath(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	//o->pathLength = 0;
	//o->nextPathSeek = -1;
	return 0;
}

static int lua_GameObject_isSelected(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	//lua_pushboolean(L, o->isSelected);
	return 1;
}

static int lua_GameObject_setSelected(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	//o->isSelected = lua_toboolean(L, 2);
	return 0;
}

static int lua_GameObject_setPathingParameters(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);

	o->pathingAccel = lua_tonumber(L, 2);
	o->pathingSpeedClamp = lua_tonumber(L, 3);
	o->smallRotationalSpeed = lua_tonumber(L, 4) * PI / 180.f;
	if (lua_tonumber(L, 5)) {
		o->largeRotationalSpeed = lua_tonumber(L, 5) * PI / 180.f;
	} else {
		o->largeRotationalSpeed = o->smallRotationalSpeed;
	}

	return 0;
}

static int lua_GameObject_setLinearVelocity(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);

	if (o->btBody) {
		F32 x = lua_tonumber(L, 2);
		F32 y = lua_tonumber(L, 3);
		F32 z = lua_tonumber(L, 4);
		o->btBody->setLinearVelocity(btVector3(x,y,z));
		o->btBody->activate(false);
	}

	return 0;
}

static int lua_GameObject_getLinearVelocity(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);

	if (o->btBody) {
		btVector3 vel = o->btBody->getLinearVelocity();
		lua_pushnumber(L, vel.x());
		lua_pushnumber(L, vel.y());
		lua_pushnumber(L, vel.z());
	} else {
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);
	}

	return 3;
}

static int lua_GameObject_getOpenGLMatrix(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 idx = 0;
	if (lua_isnumber(L, 2)) {
		idx = lua_tonumber(L, 2);
	}

	btScalar mat[16];
	if (idx == 0) {
		if (o->btBody) {
			o->btBody->getWorldTransform().getOpenGLMatrix(mat);
		} else if (o->btGhost) {
			o->btGhost->getWorldTransform().getOpenGLMatrix(mat);
		}
	} else {
		if (o->extraBodies->getSize() >= idx) {
			o->extraBodies->array[idx-1]->getWorldTransform().getOpenGLMatrix(mat);
		}
	}
	for (S32 i = 0; i < 16; i++) {
		lua_pushnumber(L, mat[i]);
	}

	return 0;
}

// params:  model index, mass
static int lua_GameObject_setMass(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);

	if (o->btBody) {
		// param 2 is the index of the model config
		S32 configIdx = lua_tointeger(L, 2);
		F32 mass = lua_tonumber(L, 3);
		if (configIdx == 0) {
			BOOL32 reAdd = FALSE;
			if ((o->btBody->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT && mass > 0) ||
					(!(o->btBody->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT) && mass) == 0) {
				// switch from static to dynamic
				reAdd = TRUE;
				static_context->world->btWorld->removeRigidBody(o->btBody);
			}
			o->btBody->setMassProps(mass, btVector3(0,0,0));
			if (reAdd) {
				static_context->world->btWorld->addRigidBody(o->btBody);
			}
		} else {
			BOOL32 reAdd = FALSE;
			if ((o->btBody->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT && mass > 0) ||
					(!(o->btBody->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT) && mass) == 0) {
				// switch from static to dynamic
				reAdd = TRUE;
				static_context->world->btWorld->removeRigidBody(o->extraBodies->array[configIdx-1]);
			}
			o->extraBodies->array[configIdx-1]->setMassProps(mass, btVector3(0,0,0));
			if (reAdd) {
				static_context->world->btWorld->addRigidBody(o->extraBodies->array[configIdx-1]);
			}
		}
	}

	return 0;
}

static int lua_GameObject_physics_allocConfigs(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	// param 2 is number of configurations
	S32 configs = lua_tointeger(L, 2);

	o->physicsModelConfigs = new ManagedArray<PhysicsModelConfig>(configs);
	for (S32 i = 0; i < configs; i++) {
		o->physicsModelConfigs->add(new PhysicsModelConfig);
	}

	return 0;
}

static void allocatePhysicsModelConfigIfNull(GameObject *o) {

	if (!o->physicsModelConfigs) {
		o->physicsModelConfigs = new ManagedArray<PhysicsModelConfig>(1);
		o->physicsModelConfigs->add(new PhysicsModelConfig);
	}

}

static int lua_GameObject_physics_createBox(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);

	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	F32 halfWidth = lua_tonumber(L, 3);
	F32 halfHeight = lua_tonumber(L, 4);
	F32 halfDepth = lua_tonumber(L, 5);
	modelConfig->shape = new btBoxShape(btVector3(halfWidth, halfHeight, halfDepth));

	return 0;
}

static int lua_GameObject_physics_addFixedHingeConstraint(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	// TODO need to support scale, rotate and translate of frame and rotate of axis somewhow.
	// This seems to only work for a fixed axis that can't be re-oriented.
	PhysicsConstraintConfig *config = new PhysicsConstraintConfig;

	config->type = HINGE_CONSTRAINT_TYPE;
	config->pivotA = btVector3(0,4,0);
	config->axisA = btVector3(0,0,1);
	o->physicsConstraintConfigs->add(config);
	// two hinge types to support:  single and anchored
	btHingeConstraint *constraint = new btHingeConstraint(*o->extraBodies->array[0], config->pivotA, config->axisA, false);
	constraint->setDbgDrawSize(10.0f);
	o->constraints->add(constraint);
	static_context->world->btWorld->addConstraint(constraint, true);

	return 0;
}

static int lua_GameObject_physics_addHingeConstraint(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	PhysicsConstraintConfig *config = new PhysicsConstraintConfig;
	S32 body0Idx = lua_tointeger(L, 2);
	S32 body1Idx = lua_tointeger(L, 3);

	config->type = HINGE_CONSTRAINT_TYPE;
	config->pivotA = btVector3(lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6));
	config->pivotB = btVector3(lua_tonumber(L, 7), lua_tonumber(L, 8), lua_tonumber(L, 9));
	config->axisA = btVector3(lua_tonumber(L, 10), lua_tonumber(L, 11), lua_tonumber(L, 12));
	config->axisB = btVector3(lua_tonumber(L, 13), lua_tonumber(L, 14), lua_tonumber(L, 15));
	bool disableCollision = true;
	if (lua_isboolean(L, 16)) {
		disableCollision = lua_toboolean(L, 16);
	}
	Vector3f mScale = o->getModelScale();
	btVector3 scale = btVector3(mScale.x, mScale.y, mScale.z);
	btTransform refA = btTransform::getIdentity();
	refA.getBasis().setEulerZYX(config->axisA.x(),config->axisA.y(),config->axisA.z());
	refA.setOrigin(config->pivotA * scale);
	btTransform refB = btTransform::getIdentity();
	refB.getBasis().setEulerZYX(config->axisB.x(),config->axisB.y(),config->axisB.z());
	refB.setOrigin(config->pivotB * scale);
	o->physicsConstraintConfigs->add(config);
	btRigidBody *body0 = NULL;
	btRigidBody *body1 = NULL;
	if (body0Idx == 0) {
		body0 = o->btBody;
	} else {
		body0 = o->extraBodies->array[body0Idx-1];
	}
	if (body1Idx == 0) {
		body1 = o->btBody;
	} else {
		body1 = o->extraBodies->array[body1Idx-1];
	}
	btHingeConstraint *constraint = new btHingeConstraint(*body0, *body1, refA, refB, false);
	constraint->setDbgDrawSize(10.0f);
	o->constraints->add(constraint);
	static_context->world->btWorld->addConstraint(constraint, disableCollision);
	lua_pushinteger(L, o->constraints->getSize()-1);

	return 1;
}

static int lua_GameObject_physics_addP2PConstraint(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	PhysicsConstraintConfig *config = new PhysicsConstraintConfig;
	S32 body0Idx = lua_tointeger(L, 2);
	S32 body1Idx = lua_tointeger(L, 3);

	config->type = POINT2POINT_CONSTRAINT_TYPE;
	config->pivotA = btVector3(lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6));
	config->pivotB = btVector3(lua_tonumber(L, 7), lua_tonumber(L, 8), lua_tonumber(L, 9));
	bool disableCollision = true;
	if (lua_isboolean(L, 10)) {
		disableCollision = lua_toboolean(L, 10);
	}
	Vector3f mScale = o->getModelScale();
	btVector3 scale = btVector3(mScale.x, mScale.y, mScale.z);
	btVector3 pivotA = config->pivotA * scale;
	btVector3 pivotB = config->pivotB * scale;
	o->physicsConstraintConfigs->add(config);
	btRigidBody *body0 = NULL;
	btRigidBody *body1 = NULL;
	if (body0Idx == 0) {
		body0 = o->btBody;
	} else {
		body0 = o->extraBodies->array[body0Idx-1];
	}
	if (body1Idx == 0) {
		body1 = o->btBody;
	} else {
		body1 = o->extraBodies->array[body1Idx-1];
	}
	btPoint2PointConstraint *constraint = new btPoint2PointConstraint(*body0, *body1, pivotA, pivotB);
	constraint->setDbgDrawSize(10.0f);
	o->constraints->add(constraint);
	static_context->world->btWorld->addConstraint(constraint, disableCollision);
	lua_pushinteger(L, o->constraints->getSize()-1);

	return 1;
}

static int lua_GameObject_physics_setConstraintLimits(lua_State *L) {

	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 idx = lua_tointeger(L, 2);
	btTypedConstraintType cType = o->physicsConstraintConfigs->array[idx]->type;
	if (cType == HINGE_CONSTRAINT_TYPE) {
		btHingeConstraint *constraint = (btHingeConstraint*) o->constraints->array[idx];
		constraint->setLimit(lua_tonumber(L, 3), lua_tonumber(L, 4));
	}

	return 0;
}

static int lua_GameObject_physics_enableConstraintMotor(lua_State *L) {

	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 idx = lua_tointeger(L, 2);
	btTypedConstraintType cType = o->physicsConstraintConfigs->array[idx]->type;
	if (cType == HINGE_CONSTRAINT_TYPE) {
		btHingeConstraint *constraint = (btHingeConstraint*) o->constraints->array[idx];
		bool enabled = lua_toboolean(L, 3);
		F32 targetVelocity = lua_tonumber(L, 4);
		F32 maxImpulse = lua_tonumber(L, 5);
		constraint->enableAngularMotor(enabled, targetVelocity, maxImpulse);
	}

	return 0;
}

static int lua_GameObject_physics_enableConstraintMotorTarget(lua_State *L) {

	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 idx = lua_tointeger(L, 2);
	btTypedConstraintType cType = o->physicsConstraintConfigs->array[idx]->type;
	if (cType == HINGE_CONSTRAINT_TYPE) {
		btHingeConstraint *constraint = (btHingeConstraint*) o->constraints->array[idx];
		bool enabled = lua_toboolean(L, 3);
		F32 targetAngle = lua_tonumber(L, 4);
		F32 dt = lua_tonumber(L, 5);
		constraint->enableMotor(enabled);
		constraint->setMotorTarget(targetAngle, dt);
	}

	return 0;
}

static int lua_GameObject_physics_createCylinder(lua_State *L) {

	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);
	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	F32 halfWidth = lua_tonumber(L, 3);
	F32 halfHeight = lua_tonumber(L, 4);
	F32 halfDepth = lua_tonumber(L, 5);
	S32 upAxis = 2; // default to Z
	if (lua_isnumber(L, 6)) {
		upAxis = lua_tointeger(L, 6);
	}
	if (upAxis == 0) {
		modelConfig->shape = new btCylinderShapeX(btVector3(halfWidth, halfHeight, halfDepth));
	} else if (upAxis == 1) {
		modelConfig->shape = new btCylinderShape(btVector3(halfWidth, halfHeight, halfDepth));
	} else {
		modelConfig->shape = new btCylinderShapeZ(btVector3(halfWidth, halfHeight, halfDepth));
	}

	return 0;
}

static int lua_GameObject_physics_createSphere(lua_State *L) {

	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);
	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	F32 radius = lua_tonumber(L, 3);
	modelConfig->shape = new btSphereShape(radius);

	return 0;
}

static int lua_GameObject_physics_setMass(lua_State *L) {

	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);
	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	F32 mass = lua_tonumber(L, 3);
	modelConfig->mass = mass;

	return 0;
}

static int lua_GameObject_physics_setPosition(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);

	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	F32 x = lua_tonumber(L, 3);
	F32 y = lua_tonumber(L, 4);
	F32 z = lua_tonumber(L, 5);
	modelConfig->position = btVector3(x,y,z);

	return 0;
}

static int lua_GameObject_physics_setFriction(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);

	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	F32 friction = lua_tonumber(L, 3);
	modelConfig->friction = friction;

	return 0;
}

static int lua_GameObject_physics_setAngularFactor(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);

	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	F32 x = lua_tonumber(L, 3);
	F32 y = lua_tonumber(L, 4);
	F32 z = lua_tonumber(L, 5);
	modelConfig->angularFactor = btVector3(x,y,z);

	return 0;
}

static int lua_GameObject_physics_setActivationState(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);

	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	F32 state = lua_tointeger(L, 3);
	modelConfig->activationState = state;

	return 0;
}

static int lua_GameObject_physics_setCollisionFilter(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);

	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	modelConfig->collisionGroup = lua_tointeger(L, 3);
	modelConfig->collisionMask = lua_tointeger(L, 4);

	return 0;
}

static int lua_GameObject_physics_setCollisionFlags(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);

	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	modelConfig->collisionFlags = lua_tointeger(L, 3);

	return 0;
}

static int lua_GameObject_physics_setGravity(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);

	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	F32 x = lua_tonumber(L, 3);
	F32 y = lua_tonumber(L, 4);
	F32 z = lua_tonumber(L, 5);
	modelConfig->gravity = btVector3(x,y,z);
	modelConfig->customGravitySet = TRUE;

	return 0;
}

static int lua_GameObject_physics_setDamping(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);

	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	F32 linearDamping = lua_tonumber(L, 3);
	F32 angularDamping = lua_tonumber(L, 4);
	modelConfig->linearDamping = linearDamping;
	modelConfig->angularDamping = angularDamping;

	return 0;
}

static int lua_GameObject_physics_setGhost(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	allocatePhysicsModelConfigIfNull(o);
	// param 2 is the index of the model config
	S32 configIdx = lua_tointeger(L, 2);
	BOOL32 isGhost = lua_toboolean(L, 3);

	PhysicsModelConfig *modelConfig = o->physicsModelConfigs->array[configIdx];
	modelConfig->isGhost = isGhost;

	return 0;
}

#endif

#ifdef BATTERYTECH_INCLUDE_BOX2D
#define CHECK_IDX_BOUNDS(idx) if (idx > GAMEOBJECT_MAX_EXTRA_BODIES) { logmsg("Error - body index out of bounds"); return 0; }

static void setFixtureDefaults(b2FixtureDef &fixDef) {
    fixDef.friction = 0.2f;
    fixDef.restitution = 0;
    fixDef.density = 1.0f;
    fixDef.isSensor = FALSE;
    fixDef.filter.categoryBits = 0x0001;
    fixDef.filter.maskBits = 0xFFFF;
    fixDef.filter.groupIndex = 0;
}

static int lua_GameObject_physics_createBody(lua_State *L) {
	// returns local ID of box body, sequentially available starting at 0
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	b2BodyDef def;
    switch (lua_tointeger(L, 2)) {
        case 0:
        	def.type = b2_staticBody;
            break;
        case 1:
        	def.type = b2_kinematicBody;
            break;
        case 2:
        	def.type = b2_dynamicBody;
            break;
        default:
            break;
    }
    if (lua_isnumber(L, 3) && lua_isnumber(L, 4)) {
        def.position = b2Vec2(lua_tonumber(L, 3), lua_tonumber(L, 4));
    } else {
    	def.position = b2Vec2_zero;
    }
 	b2Body *body = static_context->world->boxWorld->CreateBody(&def);
	body->SetUserData(o);
	for (S32 i = 0; i < GAMEOBJECT_MAX_EXTRA_BODIES; i++) {
		if (o->boxBodies->array[i] == NULL) {
			o->boxBodies->array[i] = body;
			if (i > o->boxBodies->lastItemIndex) {
				o->boxBodies->lastItemIndex = i;
			}
			lua_pushinteger(L, i);
			return 1;
		}
	}
	// bad - no more bodies allowed
	lua_pushinteger(L, -1);
	return 1;
}

static int lua_GameObject_physics_removeBody(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		b2Fixture *fixture = body->GetFixtureList();
		while (fixture) {
			o->boxFixtures->remove((S32)(size_t)fixture->GetUserData());
			fixture = fixture->GetNext();
		}
		static_context->world->boxWorld->DestroyBody(body);
		o->boxBodies->array[bodyIdx] = NULL;
		// don't mess with last item index
	}
	return 0;
}

// idx, x, y, angle
static int lua_GameObject_physics_setBodyTransform(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->SetTransform(b2Vec2(lua_tonumber(L, 3), lua_tonumber(L, 4)), lua_tonumber(L, 5));
	}
    return 0;
}

// idx, velocity
static int lua_GameObject_physics_setBodyAngularVelocity(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->SetAngularVelocity(lua_tonumber(L, 3));
	}
    return 0;
}

// idx, linearX, linearY
static int lua_GameObject_physics_setBodyLinearVelocity(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->SetLinearVelocity(b2Vec2(lua_tonumber(L, 3), lua_tonumber(L, 4)));
	}
    return 0;
}

// idx, damping
static int lua_GameObject_physics_setBodyAngularDamping(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->SetAngularDamping(lua_tonumber(L, 3));
	}
    return 0;
}

// idx, damping
static int lua_GameObject_physics_setBodyLinearDamping(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->SetLinearDamping(lua_tonumber(L, 3));
	}
    return 0;
}

// idx, allowSleep
static int lua_GameObject_physics_setBodyAllowSleep(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->SetSleepingAllowed(lua_toboolean(L, 3));
	}
    return 0;
}

// idx, isAwake
static int lua_GameObject_physics_setBodyAwake(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->SetAwake(lua_toboolean(L, 3));
	}
    return 0;
}

// idx, fixedRotation
static int lua_GameObject_physics_setBodyFixedRotation(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->SetFixedRotation(lua_toboolean(L, 3));
	}
    return 0;
}

// idx, bullet
static int lua_GameObject_physics_setBodyBullet(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->SetBullet(lua_toboolean(L, 3));
	}
    return 0;
}

// idx, active
static int lua_GameObject_physics_setBodyActive(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->SetActive(lua_toboolean(L, 3));
	}
    return 0;
}

// idx, type
static int lua_GameObject_physics_setBodyType(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
	    switch (lua_tointeger(L, 3)) {
	        case 0:
	        	body->SetType(b2_staticBody);
	            break;
	        case 1:
	        	body->SetType(b2_kinematicBody);
	            break;
	        case 2:
	        	body->SetType(b2_dynamicBody);
	            break;
	        default:
	            break;
	    }
	}
    return 0;
}

// idx, gravityscale
static int lua_GameObject_physics_setBodyGravityScale(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->SetGravityScale(lua_tonumber(L, 3));
	}
    return 0;
}


// idx
static int lua_GameObject_physics_getBodyTransform(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		b2Transform transform = body->GetTransform();
		lua_pushnumber(L, transform.p.x);
		lua_pushnumber(L, transform.p.y);
		lua_pushnumber(L, transform.q.GetAngle());
		return 3;
	}
    return 0;
}

// idx
static int lua_GameObject_physics_getBodyAngularVelocity(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		lua_pushnumber(L, body->GetAngularVelocity());
		return 1;
	}
    return 0;
}

// idx
static int lua_GameObject_physics_getBodyLinearVelocity(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		b2Vec2 vel = body->GetLinearVelocity();
		lua_pushnumber(L, vel.x);
		lua_pushnumber(L, vel.y);
		return 2;
	}
    return 0;
}

// idx, localX,localY
static int lua_GameObject_physics_getBodyWorldPoint(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		b2Vec2 p = body->GetWorldPoint(b2Vec2(lua_tonumber(L, 3), lua_tonumber(L, 4)));
		lua_pushnumber(L, p.x);
		lua_pushnumber(L, p.y);
		return 2;
	}
    return 0;
}

// bodyId, pointCount, p1x,p1y, etc
static int lua_GameObject_physics_getBodyWorldPoints(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
        S32 pointCount = lua_tointeger(L, 3);
        for (S32 i = 0; i < pointCount; i++) {
            F32 x = lua_tonumber(L, 4 + i*2);
            F32 y = lua_tonumber(L, 4 + i*2+1);
            b2Vec2 p = body->GetWorldPoint(b2Vec2(x,y));
            lua_pushnumber(L, p.x);
            lua_pushnumber(L, p.y);
        }
		return pointCount * 2;
	}
    return 0;
}


static int lua_GameObject_physics_getBodyLocalPoint(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		b2Vec2 p = body->GetLocalPoint(b2Vec2(lua_tonumber(L, 3), lua_tonumber(L, 4)));
		lua_pushnumber(L, p.x);
		lua_pushnumber(L, p.y);
		return 2;
	}
    return 0;
}

static int lua_GameObject_physics_getBodyLocalCenter(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		b2Vec2 p = body->GetLocalCenter();
		lua_pushnumber(L, p.x);
		lua_pushnumber(L, p.y);
		return 2;
	}
    return 0;
}

static int lua_GameObject_physics_applyForce(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		F32 forceX = lua_tonumber(L, 3);
		F32 forceY = lua_tonumber(L, 4);
		F32 pointX = lua_tonumber(L, 5);
		F32 pointY = lua_tonumber(L, 6);
		body->ApplyForce(b2Vec2(forceX, forceY), b2Vec2(pointX, pointY));
	}
    return 0;
}

static int lua_GameObject_physics_applyTorque(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->ApplyTorque(lua_tonumber(L, 3));
	}
    return 0;
}

static int lua_GameObject_physics_applyLinearImpulse(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		F32 forceX = lua_tonumber(L, 3);
		F32 forceY = lua_tonumber(L, 4);
		F32 pointX = lua_tonumber(L, 5);
		F32 pointY = lua_tonumber(L, 6);
		body->ApplyLinearImpulse(b2Vec2(forceX, forceY), b2Vec2(pointX, pointY));
	}
    return 0;
}

static int lua_GameObject_physics_applyAngularImpulse(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		body->ApplyAngularImpulse(lua_tonumber(L, 3));
	}
    return 0;
}

// bodyId, x0,y0, x1,y1, x2,y2, x3,y3, etc
static int lua_GameObject_physics_createPolygonFixture(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
	    b2Vec2 verts[12];
	    // for each point pair, add
	    S32 n = 3;
	    S32 pointCount = 0;
	    b2PolygonShape *shape = new b2PolygonShape;
	    while (lua_isnumber(L, n) && lua_isnumber(L, n+1)) {
	        verts[pointCount] = b2Vec2(lua_tonumber(L, n), lua_tonumber(L, n+1));
	        pointCount++;
	        n+=2;
	        if (pointCount == 12) {
	            break;
	        }
	    }
	    shape->Set(verts, pointCount);
	    b2FixtureDef fixDef;
	    setFixtureDefaults(fixDef);
	    fixDef.shape = shape;
	    b2Fixture *fixture = body->CreateFixture(&fixDef);
	    S32 id = o->nextFixtureId++;
	    fixture->SetUserData((void*)id);
	    o->boxFixtures->put(id, fixture);
	    lua_pushinteger(L, id);
	    return 1;
	}
	return 0;
}

// bodyId, radius, (optional body offset x,y)
static int lua_GameObject_physics_createCircleFixture(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		// param 3 is radius
		b2CircleShape *shape = new b2CircleShape;
		shape->m_radius = lua_tonumber(L, 3);
		if (lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
			shape->m_p = b2Vec2(lua_tonumber(L, 4), lua_tonumber(L, 5));
		}
	    b2FixtureDef fixDef;
	    setFixtureDefaults(fixDef);
	    fixDef.shape = shape;
	    b2Fixture *fixture = body->CreateFixture(&fixDef);
	    S32 id = o->nextFixtureId++;
	    fixture->SetUserData((void*)id);
	    o->boxFixtures->put(id, fixture);
	    lua_pushinteger(L, id);
	    return 1;
	}
	return 0;
}

// bodyId, loop, x1,y1, x2,y2
static int lua_GameObject_physics_createChainFixture(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	if (body) {
		BOOL32 loop = lua_toboolean(L, 3);
	    b2Vec2 verts[1024];
	    // for each point pair, add
	    S32 n = 4;
	    S32 pointCount = 0;
	    b2ChainShape *shape = new b2ChainShape;
	    while (lua_isnumber(L, n) && lua_isnumber(L, n+1)) {
	        verts[pointCount] = b2Vec2(lua_tonumber(L, n), lua_tonumber(L, n+1));
	        pointCount++;
	        n+=2;
	        if (pointCount == 1024) {
	            break;
	        }
	    }
	    if (loop) {
	    	shape->CreateLoop(verts, pointCount);
	    } else {
		    shape->CreateChain(verts, pointCount);
	    }
	    b2FixtureDef fixDef;
	    setFixtureDefaults(fixDef);
	    fixDef.shape = shape;
	    b2Fixture *fixture = body->CreateFixture(&fixDef);
	    S32 id = o->nextFixtureId++;
	    fixture->SetUserData((void*)id);
	    o->boxFixtures->put(id, fixture);
	    lua_pushinteger(L, id);
	    return 1;
	}
	return 0;
}

// fixture id
static int lua_GameObject_physics_removeFixture(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 fixtureId = lua_tointeger(L, 2);
	b2Fixture *fixture = o->boxFixtures->remove(fixtureId);
	if (fixture) {
		fixture->GetBody()->DestroyFixture(fixture);
	}
	return 0;
}

// fixture id, density
static int lua_GameObject_physics_setFixtureDensity(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 fixtureId = lua_tointeger(L, 2);
	b2Fixture *fixture = o->boxFixtures->get(fixtureId);
	if (fixture) {
		fixture->SetDensity(lua_tonumber(L,3));
		fixture->GetBody()->ResetMassData();
	}
    return 0;
}

// fixture id, friction
static int lua_GameObject_physics_setFixtureFriction(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 fixtureId = lua_tointeger(L, 2);
	b2Fixture *fixture = o->boxFixtures->get(fixtureId);
	if (fixture) {
		fixture->SetFriction(lua_tonumber(L,3));
	}
    return 0;
}

// fixture id, restitution
static int lua_GameObject_physics_setFixtureRestitution(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 fixtureId = lua_tointeger(L, 2);
	b2Fixture *fixture = o->boxFixtures->get(fixtureId);
	if (fixture) {
		fixture->SetRestitution(lua_tonumber(L,3));
	}
    return 0;
}

// fixture id, isSensor
static int lua_GameObject_physics_setFixtureIsSensor(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 fixtureId = lua_tointeger(L, 2);
	b2Fixture *fixture = o->boxFixtures->get(fixtureId);
	if (fixture) {
		fixture->SetSensor(lua_toboolean(L, 3));
	}
    return 0;
}

// fixture id, category, mask, group
static int lua_GameObject_physics_setFixtureFilter(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 fixtureId = lua_tointeger(L, 2);
	b2Fixture *fixture = o->boxFixtures->get(fixtureId);
	if (fixture) {
		b2Filter filter;
		filter.categoryBits = (U16)lua_tointeger(L, 3);
		filter.maskBits = (U16)lua_tointeger(L, 4);
		filter.groupIndex = (S16)lua_tointeger(L, 5);
		fixture->SetFilterData(filter);
	}
    return 0;
}

static int lua_GameObject_physics_fixtureTestPoint(lua_State *L) {
	GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 fixtureId = lua_tointeger(L, 2);
	b2Fixture *fixture = o->boxFixtures->get(fixtureId);
	if (fixture) {
		F32 x = lua_tonumber(L, 3);
		F32 y = lua_tonumber(L, 4);
		lua_pushboolean(L, fixture->TestPoint(b2Vec2(x,y)));
		return 1;
	}
    return 0;
}

static int lua_GameObject_physics_getFixtureBodyId(lua_State *L) {
    GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 fixtureId = lua_tointeger(L, 2);
	b2Fixture *fixture = o->boxFixtures->get(fixtureId);
	if (fixture) {
        b2Body *body = fixture->GetBody();
        for (S32 i = 0; i < o->boxBodies->getSize(); i++) {
            if (o->boxBodies->array[i] == body) {
                lua_pushinteger(L, i);
                return 1;
            }
        }
	}
    return 0;
}


static int lua_GameObject_queryPhysicsContacts(lua_State *L) {
	// 2 = bodyIdx
	// 3 = function name to callback details to - signature is func(otherObj, otherFixtureId, pointCount, x1,y1, x2,y2)
	// returns number of contacts
    GameObject *o = *(GameObject**)lua_touserdata(L, 1);
	S32 bodyIdx = lua_tointeger(L, 2);
	const char *functionName = lua_tostring(L, 3);
	CHECK_IDX_BOUNDS(bodyIdx);
	b2Body *body = o->boxBodies->array[bodyIdx];
	S32 count = 0;
	if (body) {
		for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next) {
			b2Contact* c = ce->contact;
			if (c->IsEnabled() && c->IsTouching()) {
				b2Fixture *other = NULL;
				if (c->GetFixtureA()->GetBody()->GetUserData() == o) {
					other = c->GetFixtureB();
				} else {
					other = c->GetFixtureA();
				}
				b2Body *otherBody = other->GetBody();
				GameObject *otherObj = (GameObject*) otherBody->GetUserData();
				b2Manifold *manifold = c->GetManifold();
				count++;
				if (functionName) {
					if (o->luaBinder->pushInstanceFunction(functionName)) {
						// calls func(otherObj, otherFixtureId, pointCount, x1,y1, x2,y2)
						o->luaBinder->callFunctionVA("GameObject:<callback function>", TRUE, "riidddd>",
								otherObj->luaBinder->luaRef, (size_t)other->GetUserData(), manifold->pointCount, manifold->points[0].localPoint.x, manifold->points[0].localPoint.y,
								manifold->points[0].localPoint.x, manifold->points[0].localPoint.y);
					}
				}
			}
		}
	}
	lua_pushinteger(L, count);
    return 1;
}

// 0 = none, 1 = broad, 2 = narrow
static int lua_GameObject_setPhysicsCallbackDetail(lua_State *L) {
    GameObject *o = *(GameObject**)lua_touserdata(L, 1);
    S32 detail = lua_tointeger(L, 2);
    if (detail == 1) {
        o->callbackDetail = CALLBACK_DETAIL_BROAD;
    } else if (detail == 2) {
        o->callbackDetail = CALLBACK_DETAIL_NARROW;
    } else {
        o->callbackDetail = CALLBACK_DETAIL_NONE;
    }
    return 0;
}

static int lua_GameObject_countPhysicsContacts(lua_State *L) {
    GameObject *o = *(GameObject**)lua_touserdata(L, 1);
    lua_pushinteger(L, o->contactsUsed);
    return 1;
}

// returns GameObject__count__lx0__ly0__lx1__ly1__isTouching__isActive__fixtureId
static int lua_GameObject_getPhysicsContact(lua_State *L) {
    GameObject *o = *(GameObject**)lua_touserdata(L, 1);
    S32 idx = lua_tointeger(L, 2);
    if (idx > o->contactsUsed-1) {
    	return 0;
    }
    GameObject::PhysicsContact2D *pc = o->contacts->array[idx];
	lua_rawgeti( L, LUA_REGISTRYINDEX, pc->other->luaBinder->luaRef ); // push userdata of other gameobject via reference
    lua_pushinteger(L, pc->pointCount);
    lua_pushnumber(L, pc->localPoint[0].x);
    lua_pushnumber(L, pc->localPoint[0].y);
    lua_pushnumber(L, pc->localPoint[1].x);
    lua_pushnumber(L, pc->localPoint[1].y);
    lua_pushboolean(L, pc->isTouching);
    lua_pushboolean(L, pc->isActive);
    lua_pushinteger(L, (size_t)pc->fixture->GetUserData());
    return 8;
}

#endif

static int lua_GameObject_gc (lua_State *L) {
	//char buf[255];
	//sprintf(buf, "**GC** GameObject (%p)\n", *(GameObject**)lua_touserdata(L, 1));
	//logmsg(buf);
	return 0;
}

static int lua_GameObject_tostring (lua_State *L) {
  lua_pushfstring(L, "GameObject: %p", *(GameObject**)lua_touserdata(L, 1));
  return 1;
}

