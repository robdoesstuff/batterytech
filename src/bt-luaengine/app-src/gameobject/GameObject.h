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

#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include "../PhysicsBodyObject.h"
#include <batterytech/primitives.h>
#include "../GameConstants.h"
#include "../GameContext.h"
#include <stdio.h>
#include <batterytech/util/TextFileUtil.h>
#include <batterytech/math/Vector2.h>
#include <batterytech/util/ManagedArray.h>
#include <batterytech/math/Vector3.h>
#ifdef BATTERYTECH_INCLUDE_BOX2D
#include <bt-box2d/Dynamics/Contacts/b2Contact.h>
#include <bt-box2d/Dynamics/b2WorldCallbacks.h>
#include "../GameConstants.h"
#include <bt-box2d/Dynamics/b2Body.h>
#endif

#define GAMEOBJECT_MAX_PATH 100
#define GAMEOBJECT_MAX_CONTACTS 16
#define GAMEOBJECT_MAX_EXTRA_BODIES 10
#define GAMEOBJECT_MAX_FIXTURES 100
#define GAMEOBJECT_MAX_CONSTRAINTS 10

#define GAMEOBJECT_SMALL_ROTATIONAL_SPEED 90.0f
#define GAMEOBJECT_LARGE_ROTATIONAL_SPEED 360.0f
#define GAMEOBJECT_PATHING_ACCEL 12.0f
#define GAMEOBJECT_PATHING_SPEED_CLAMP 4.0f
// In radians - any angle this or smaller is considered small rotation, otherwise is large, this should be 45 degrees
#define GAMEOBJECT_SMALL_ROTATION_THRESHOLD 0.7f

class World;
class GameObjectMeta;

namespace BatteryTech { class AssimpAnimator; }

class btRigidBody;
class GameObjectLuaBinder;

using namespace BatteryTech;

enum PhysicsCallbackDetail { CALLBACK_DETAIL_NONE = 0, CALLBACK_DETAIL_BROAD, CALLBACK_DETAIL_NARROW };

struct PhysicsModelConfig {
#ifdef BATTERYTECH_INCLUDE_BULLET
	btCollisionShape* shape;
	btScalar mass;
	btVector3 position;
	btScalar friction;
	btVector3 angularFactor;
	S32 activationState;
	S16 collisionGroup;
	S16 collisionMask;
	BOOL32 customGravitySet;
	btVector3 gravity;
	btScalar linearDamping;
	btScalar angularDamping;
	BOOL32 isGhost;
	S32 collisionFlags;
	PhysicsModelConfig() {
		shape = NULL;
		mass = 0;
		position = btVector3(0,0,0);
		friction = 0.5f;
		angularFactor = btVector3(1,1,1);
		activationState = ACTIVE_TAG;
		collisionGroup = 1;
		collisionMask = 0xffff;
		collisionFlags = 0;
		customGravitySet = FALSE;
		gravity = btVector3(0,0,0);
		linearDamping = 0;
		angularDamping = 0;
		isGhost = FALSE;
	}
#endif
};

struct PhysicsConstraintConfig {
#ifdef BATTERYTECH_INCLUDE_BULLET
	btTypedConstraintType type;
	S32 idxA;
	S32 idxB;
	btVector3 pivotA;
	btVector3 pivotB;
	btVector3 axisA;
	btVector3 axisB;
	PhysicsConstraintConfig() {
		type = MAX_CONSTRAINT_TYPE;
		idxA = -1;
		idxB = -1;
		pivotA = btVector3(0,0,0);
		pivotB = btVector3(0,0,0);
		axisA = btVector3(0,0,0);
		axisB = btVector3(0,0,0);
	}
#endif
};

class GameObject : PhysicsBodyObject {
public:
    
	struct PathNode {
		Vector3f pos;
		GameObject *attachment;
		PathNode() : pos(Vector3f(0,0,0)), attachment(NULL){}
		PathNode(Vector3f pos, GameObject *attachment) {
			this->pos = pos;
			this->attachment = attachment;
		}
		virtual ~PathNode() {
			attachment = NULL;
		}
	};
	GameObject(GameContext *context);
	virtual ~GameObject();
	void init();
	void setPosition(Vector3f pos);
	Vector3f getPosition();
	void setModelScale(Vector3f modelScale);
	Vector3f getModelScale();
	void update();
	BOOL32 isValidGameObject(GameObject *other);
    void onOtherGameObjectRemoved(GameObject *other);
	// if the gameobject is still active - will be removed from world if not
	BOOL32 isActive;
	// location of game object in game world coordinates
	Vector3f pos;
	// location of game object at last update
	Vector3f lastPos;
	GameObjectLuaBinder *luaBinder;
	ManagedArray<AssimpAnimator> *animators;
	Vector3f modelScale;
	BOOL32 isInitialized;
    // destroys the b2Body attached to a gameobject
	void destroyBody();
#ifdef BATTERYTECH_INCLUDE_BOX2D
    // multiple fixtures of the same gameobject may be contacting each with up to 2 points
    struct PhysicsContact2D {
        Vector2f localPoint[2];
        S32 pointCount;
        GameObject *other;
        b2Fixture *fixture;
        BOOL32 wasTouching;
        BOOL32 isTouching;
        BOOL32 isActive;
        BOOL32 callbackProcessed;
        F32 impulse;
        F32 approachVelocity;
        PhysicsContact2D() {
            localPoint[0] = Vector2f(0,0);
            localPoint[1] = Vector2f(0,0);
            pointCount = 0;
            other = NULL;
            fixture = NULL;
            wasTouching = FALSE;
            isTouching = FALSE;
            isActive = FALSE;
            callbackProcessed = FALSE;
            impulse = 0;
            approachVelocity = 0;
        }
    };
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
 	ManagedArray<b2Body> *boxBodies;
 	HashTable<S32, b2Fixture*> *boxFixtures;
 	S32 nextFixtureId;
    // information about the physics contacts which we use to make callbacks
    ManagedArray<PhysicsContact2D> *contacts;
    // a hashed set of pointers to gameobjects that we've notified the script about contacts
    HashTable<GameObject*, GameObject*> *contactObjects;
    S32 contactsUsed;
    PhysicsCallbackDetail callbackDetail;
#endif
#ifdef BATTERYTECH_INCLUDE_BULLET
	void setAllOrientationYPR(Vector3f ypr);
	void setOrientationYPR(S32 idx, Vector3f ypr);
	Vector3f getOrientationYPR(S32 idx = 0);
	void updatePathing();
	// moves or rotates this object
	void transform(F32 x, F32 y, F32 angle){};
	// called when this object is touched (x and y are in world-coordinates)
	void onTouchDown(F32 x, F32 y){};
	// called when the touch on this object is moved (x and y are in world-coordinates)
	void onTouchMove(F32 x, F32 y){};
	// called when the touch on this object is lifted (x and y are in world-coordinates)
	void onTouchUp(F32 x, F32 y){};
	F32 getLinearVelocity() { return 0.0f; };
	void startCollisionPhase();
	void onCollision(GameObject *other);
	void onLevelCollision();
	void endCollisionPhase();
	// the primary body of this gameobject (be careful messing with this - it may not be the main one!)
	btRigidBody *btBody;
	btGhostObject *btGhost;
	btDefaultMotionState* myMotionState;
	ManagedArray<btRigidBody> *extraBodies;
	ManagedArray<btDefaultMotionState> *extraMotionStates;
	ManagedArray<btTypedConstraint> *constraints;
	BOOL32 isSelected;
	PathNode path[GAMEOBJECT_MAX_PATH];
	S32 pathLength;
	S32 nextPathSeek;
	ManagedArray<PhysicsModelConfig> *physicsModelConfigs;
	ManagedArray<PhysicsConstraintConfig> *physicsConstraintConfigs;
	F32 zRotation;
	F32 smallRotationalSpeed;
	F32 largeRotationalSpeed;
	F32 pathingAccel;
	F32 pathingSpeedClamp;
	ManagedArray<GameObject> *contacts;
	ManagedArray<GameObject> *oldContacts;
	BOOL32 wasLevelColliding;
	BOOL32 isLevelColliding;
#endif
protected:
	// shortcut to context->world
	World* getWorld();
	// a reference to the game context
	GameContext *context;
#ifdef BATTERYTECH_INCLUDE_BOX2D
    PhysicsContact2D* findContact(b2Contact* contact);
#endif
};

#endif /* GAMEOBJECT_H_ */
