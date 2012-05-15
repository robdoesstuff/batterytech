/*
 * GameObject.h
 *
 *  Created on: Nov 2, 2010
 *      Author: rgreen
 */

#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include "../PhysicsBodyObject.h"
#include <batterytech/primitives.h>
#include <bt-box2d/Dynamics/Contacts/b2Contact.h>
#include <bt-box2d/Dynamics/b2WorldCallbacks.h>
#include "../GameConstants.h"
#include <bt-box2d/Dynamics/b2Body.h>
#include "../GameContext.h"
#include <stdio.h>
#include <batterytech/util/TextFileUtil.h>
#include <batterytech/math/Vector2.h>
#include <batterytech/util/ManagedArray.h>
#include "GameObjectProperty.h"

#define GAMEOBJECT_NO_USER_ID -1
#define GAMEOBJECT_NO_USER_GROUP_ID -1

class World;
class GameObjectMeta;
class GameContext;

using namespace BatteryTech;

class GameObject : PhysicsBodyObject {
public:
	GameObject(GameContext *context);
	virtual ~GameObject();
	virtual void update(){};
	// called when contact has started with another game object
	// do NOT modify the contents of boxWorld here!  It will crash
	virtual void contactStarted(b2Contact* contact);
	// called when contact has ended with another game object
	// do NOT modify the contents of boxWorld here!  It will crash
	virtual void contactEnded(b2Contact* contact);
	virtual void contactPreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void contactPostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	/// Called when any joint is about to be destroyed due
	/// to the destruction of one of its attached bodies.
	virtual void SayGoodbye(b2Joint* joint){};
	/// Called when any fixture is about to be destroyed due
	/// to the destruction of its parent body.
	virtual void SayGoodbye(b2Fixture* fixture){};
	// destroys the b2Body attached to a gameobject
	virtual void destroyBody();
	// moves or rotates this object
	virtual void transform(F32 x, F32 y, F32 angle){};
	// called when this object is touched (x and y are in world-coordinates)
	virtual void onTouchDown(F32 x, F32 y){};
	// called when the touch on this object is moved (x and y are in world-coordinates)
	virtual void onTouchMove(F32 x, F32 y){};
	// called when the touch on this object is lifted (x and y are in world-coordinates)
	virtual void onTouchUp(F32 x, F32 y){};
	// stores this game object's properties to a file (starting at current position, stopping at closing bracket)
	virtual void storeTo(FILE *file){};
	// loads this game object's properties from a block of text
	virtual void loadFrom(char* text){};
	virtual F32 getLinearVelocity() { return 0.0f; };
	// resets the impact solution
	virtual void clearImpact();
	virtual void setProperty(const char *name, const char *value);
	virtual ManagedArray<GameObjectProperty>* getProperties();
	virtual const char* getProperty(const char *name);
	// instance method of getMeta
	virtual GameObjectMeta* getIMeta()=0;
	// if the gameobject is still active - will be removed from world if not
	BOOL32 isActive;
	// location of game object in game world coordinates
	F32 x,y,z;
	// object's rotation in radians
	F32 angle;
	// the type of game object - see GameConstants.h for available types
	U8 type;
	// the primary box2d body of this gameobject (be careful messing with this - it may not be the main one!)
	b2Body *boxBody;
	S32 userId;
	S32 userGroupId;
protected:
	// calculates the center point of a concave polygon comprised of points
	static Vector2f calcCenter(Vector2f *points, S32 count);
	// shortcut to context->world
	World* getWorld();
	// handed a line of loadable text, this will attempt to match a property and load it if it matches.
	virtual void loadPropertyFrom(char* line);
	virtual void storePropertiesTo(FILE *file);
	virtual void sendEvent(const char* eventName);
	// a reference to the game context
	GameContext *context;
	BOOL32 processContact;
	F32 preSolveVelocity;
	F32 postSolveVelocity;
	F32 impactVelocityDelta;
	ManagedArray<GameObjectProperty> *properties;
};

/* I know this seems a little insane but forward-declaring World and including at the end
 * as a catch-all is the only way to solve the circular dependancies caused by GameObject subclasses */
#include "../World.h"

#endif /* GAMEOBJECT_H_ */
