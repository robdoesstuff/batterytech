/*
 * GameObject.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: rgreen
 */

#include "GameObject.h"
#include <string.h>
#include <batterytech/Logger.h>
#include "../World.h"
#include "GameObjectLuaBinder.h"
#include <math.h>
#include <batterytech/render/AssimpAnimator.h>

using namespace BatteryTech;

/**
 * GameObject has a primary rigid body, an array of other bodies and an array of constraints.
 * The lua binder can address any body or constraint using the array indices.
 * Rendering needs to know which body to use for worldtransform.
 *
 */

GameObject::GameObject(GameContext *context) : PhysicsBodyObject(PHYSICS_BODY_TYPE_GAMEOBJECT) {
	this->context = context;
	isInitialized = FALSE;
	isActive = TRUE;
	luaBinder = new GameObjectLuaBinder(context->world->luaState, this, context);
	modelScale = Vector3f(1,1,1);
	pos = Vector3f(0,0,0);
	lastPos = Vector3f(0,0,0);
	animators = NULL;
#ifdef BATTERYTECH_INCLUDE_BOX2D
    boxBody = NULL;
    processContact = FALSE;
	preSolveVelocity = 0;
	postSolveVelocity = 0;
	impactVelocityDelta = 0;
#endif
	/*
	btBody = NULL;
	btGhost = NULL;
	isSelected = FALSE;
	pathLength = 0;
	nextPathSeek = -1;
	physicsModelConfigs = NULL;
	zRotation = 0;
	smallRotationalSpeed = GAMEOBJECT_SMALL_ROTATIONAL_SPEED * PI / 180.f;
	largeRotationalSpeed = GAMEOBJECT_LARGE_ROTATIONAL_SPEED * PI / 180.f;
	pathingAccel = GAMEOBJECT_PATHING_ACCEL;
	pathingSpeedClamp = GAMEOBJECT_PATHING_SPEED_CLAMP;
	contacts = new ManagedArray<GameObject>(GAMEOBJECT_MAX_CONTACTS);
	oldContacts = new ManagedArray<GameObject>(GAMEOBJECT_MAX_CONTACTS);
	extraBodies = new ManagedArray<btRigidBody>(GAMEOBJECT_MAX_EXTRA_BODIES);
	extraMotionStates = new ManagedArray<btDefaultMotionState>(GAMEOBJECT_MAX_EXTRA_BODIES);
	constraints = new ManagedArray<btTypedConstraint>(GAMEOBJECT_MAX_CONSTRAINTS);
	physicsConstraintConfigs = new ManagedArray<PhysicsConstraintConfig>(GAMEOBJECT_MAX_CONSTRAINTS);
	isLevelColliding = FALSE;
	wasLevelColliding = FALSE;
	*/
}

GameObject::~GameObject() {
	/*
	destroyBody();
	if (physicsModelConfigs) {
		physicsModelConfigs->deleteElements();
		delete physicsModelConfigs;
	}
	physicsModelConfigs = NULL;
	physicsConstraintConfigs->deleteElements();
	delete physicsConstraintConfigs;
	physicsConstraintConfigs = NULL;
	delete constraints;
	constraints = NULL;
	*/
	delete luaBinder;
	luaBinder = NULL;
	context = NULL;
	if (animators) {
		animators->deleteElements();
		delete animators;
	}
	animators = NULL;
}

#ifdef BATTERYTECH_INCLUDE_BOX2D
void GameObject::contactStarted(b2Contact* contact) {
}

void GameObject::contactEnded(b2Contact* contact) {
}

void GameObject::contactPreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
	::PhysicsBodyObject *objA = (::PhysicsBodyObject*) contact->GetFixtureA()->GetBody()->GetUserData();
	::PhysicsBodyObject *objB = (::PhysicsBodyObject*) contact->GetFixtureB()->GetBody()->GetUserData();
	if (objA->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
		GameObject *gObjA = (GameObject*) objA;
        if (gObjA == this) {
            preSolveVelocity = gObjA->getLinearVelocity();
        }
	}
	if (objB->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
		GameObject *gObjB = (GameObject*) objB;
        if (gObjB == this) {
            preSolveVelocity = gObjB->getLinearVelocity();
        }
	}
}

void GameObject::contactPostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
	::PhysicsBodyObject *objA = (::PhysicsBodyObject*) contact->GetFixtureA()->GetBody()->GetUserData();
	::PhysicsBodyObject *objB = (::PhysicsBodyObject*) contact->GetFixtureB()->GetBody()->GetUserData();
	if (objA->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
		GameObject *gObjA = (GameObject*) objA;
		if (gObjA == this) {
			postSolveVelocity = gObjA->getLinearVelocity();
			impactVelocityDelta += preSolveVelocity - postSolveVelocity;
			processContact = TRUE;
		}
	}
	if (objB->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
		GameObject *gObjB = (GameObject*) objB;
		if (gObjB == this) {
			postSolveVelocity = gObjB->getLinearVelocity();
			impactVelocityDelta += preSolveVelocity - postSolveVelocity;
			processContact = TRUE;
		}
	}
}

void GameObject::clearImpact() {
	processContact = FALSE;
	preSolveVelocity = 0;
	postSolveVelocity = 0;
	impactVelocityDelta = 0;
}
#endif

void GameObject::setPosition(Vector3f pos) {
	this->pos = pos;
	if (this->lastPos == Vector3f(0,0,0)) {
		this->lastPos = pos;
	}
	/*
	if (physicsModelConfigs) {
		for (S32 i = 0; i < physicsModelConfigs->getSize(); i++) {
			PhysicsModelConfig *config = physicsModelConfigs->array[i];
			//btVector3 newPos = btVector3(pos.x + config->position.x() * modelScale.x, pos.y + config->position.y() * modelScale.y, pos.z + config->position.z() * modelScale.z);
			btRigidBody *btBody = NULL;
			btGhostObject *btGhost = NULL;
			btDefaultMotionState *motionState = NULL;
			if (i == 0) {
				btBody = this->btBody;
				btGhost = this->btGhost;
				motionState = this->myMotionState;
			} else {
				btBody = extraBodies->array[i - 1];
				motionState = extraMotionStates->array[i - 1];
			}
			if (btBody) {
				btVector3 newPos = btBody->getWorldTransform().getBasis() * btVector3(config->position.x() * modelScale.x, config->position.y() * modelScale.y, config->position.z() * modelScale.z);
				newPos += btVector3(pos.x, pos.y, pos.z);
				btTransform trans = btBody->getWorldTransform();
				trans.setOrigin(newPos);
				btBody->setWorldTransform(trans);
				motionState->getWorldTransform(trans);
				trans.setOrigin(newPos);
				motionState->setWorldTransform(trans);
			} else if (btGhost) {
				btMatrix3x3 mat = btGhost->getWorldTransform().getBasis();
				btVector3 newPos = mat * btVector3(config->position.x() * modelScale.x, config->position.y() * modelScale.y, config->position.z() * modelScale.z);
				newPos += btVector3(pos.x, pos.y, pos.z);
				btGhost->getWorldTransform().setOrigin(newPos);
			}
		}
	}
	*/
}

Vector3f GameObject::getPosition() {
	return pos;
}

/*
void GameObject::setAllOrientationYPR(Vector3f ypr) {
	F32 y = ypr.x;
	F32 p = ypr.y;
	F32 r = ypr.z;
	zRotation = y; // yaw = z
	btRigidBody *btBody = NULL;
	btGhostObject *btGhost = NULL;
	btDefaultMotionState *motionState = NULL;
	btMatrix3x3 basis;
	for (S32 i = 0; i < physicsModelConfigs->getSize(); i++) {
		if (i == 0) {
			btBody = this->btBody;
			btGhost = this->btGhost;
			motionState = this->myMotionState;
		} else {
			btBody = extraBodies->array[i - 1];
			motionState = extraMotionStates->array[i - 1];
		}
		if (btBody) {
			basis = btBody->getWorldTransform().getBasis();
			basis.setEulerYPR(y, p, r);
			btBody->getWorldTransform().setBasis(basis);
			btBody->activate(false);
			btTransform trans;
			motionState->getWorldTransform(trans);
			trans.getBasis().setEulerYPR(y, p, r);
			motionState->setWorldTransform(trans);
		} else if (btGhost) {
			basis = btGhost->getWorldTransform().getBasis();
			basis.setEulerYPR(y, p, r);
			btGhost->getWorldTransform().setBasis(basis);
		}
	}
	// re-setting the position will adjust the position relative to the new offset scale
	setPosition(pos);
}

void GameObject::setOrientationYPR(S32 idx, Vector3f ypr) {
	F32 y = ypr.x;
	F32 p = ypr.y;
	F32 r = ypr.z;
	zRotation = y; // yaw = z
	btRigidBody *btBody = NULL;
	btGhostObject *btGhost = NULL;
	btDefaultMotionState *motionState = NULL;
	btMatrix3x3 basis;
	for (S32 i = 0; i < physicsModelConfigs->getSize(); i++) {
		if (i == idx) {
			if (i == 0) {
				btBody = this->btBody;
				btGhost = this->btGhost;
				motionState = this->myMotionState;
			} else {
				btBody = extraBodies->array[i - 1];
				motionState = extraMotionStates->array[i - 1];
			}
			if (btBody) {
				basis = btBody->getWorldTransform().getBasis();
				basis.setEulerYPR(y, p, r);
				btBody->getWorldTransform().setBasis(basis);
				btBody->activate(false);
				btTransform trans;
				motionState->getWorldTransform(trans);
				trans.getBasis().setEulerYPR(y, p, r);
				motionState->setWorldTransform(trans);
			} else if (btGhost) {
				basis = btGhost->getWorldTransform().getBasis();
				basis.setEulerYPR(y, p, r);
				btGhost->getWorldTransform().setBasis(basis);
			}
			break;
		}
	}
	// re-setting the position will adjust the position relative to the new offset scale
	setPosition(pos);
}

Vector3f GameObject::getOrientationYPR(S32 index) {
	if (index == 0) {
		if (btBody) {
			F32 y, p, r;
			btBody->getWorldTransform().getBasis().getEulerYPR(y, p, r);
			return Vector3f(y,p,r);
		} else if (btGhost) {
			F32 y, p, r;
			btGhost->getWorldTransform().getBasis().getEulerYPR(y, p, r);
			return Vector3f(y,p,r);
		}
	} else {
		if (extraBodies->getSize() >= index) {
			F32 y, p, r;
			extraBodies->array[index-1]->getWorldTransform().getBasis().getEulerYPR(y, p, r);
			return Vector3f(y,p,r);
		}
	}
	return Vector3f(0,0,0);
}
*/

void GameObject::setModelScale(Vector3f scale) {
	this->modelScale = scale;
	/*
	if (btBody || btGhost) {
		for (S32 i = 0; i < physicsModelConfigs->getSize(); i++) {
			btVector3 scaleBtVec = btVector3(scale.x, scale.y, scale.z);
			physicsModelConfigs->array[i]->shape->setLocalScaling(scaleBtVec);
		}
		// re-setting the position will adjust the position relative to the new offset scale
		setPosition(pos);
		btVector3 btScale = btVector3(scale.x, scale.y, scale.z);
		for (S32 i = 0; i < physicsConstraintConfigs->getSize(); i++) {
			PhysicsConstraintConfig *config = physicsConstraintConfigs->array[i];
			btTypedConstraintType type = config->type;
			if (type == HINGE_CONSTRAINT_TYPE) {
				btHingeConstraint *constraint = (btHingeConstraint*)constraints->array[i];
				btTransform refA = constraint->getFrameOffsetA();
				btTransform refB = constraint->getFrameOffsetB();
				refA.setOrigin(config->pivotA * btScale);
				refB.setOrigin(config->pivotB * btScale);
				constraint->setFrames(refA, refB);
			} else if (type == POINT2POINT_CONSTRAINT_TYPE) {
				btPoint2PointConstraint *constraint = (btPoint2PointConstraint*)constraints->array[i];
				constraint->setPivotA(config->pivotA * btScale);
				constraint->setPivotB(config->pivotB * btScale);
			}
		}
	}
*/
}

Vector3f GameObject::getModelScale() {
	return modelScale;
}

void GameObject::init() {
	/*
	for (S32 i = 0; i < physicsModelConfigs->getSize(); i++) {
		PhysicsModelConfig *config = physicsModelConfigs->array[i];
		btVector3 localInertia(0,0,0);
		if (config->mass != 0.0f) {
			config->shape->calculateLocalInertia(config->mass, localInertia);
		}
		config->shape->setLocalScaling(btVector3(modelScale.x, modelScale.y, modelScale.z));
		btTransform startTransform;
		startTransform.setIdentity();
		// origin is at the GameObject's position plus the configuration's positional offset
		startTransform.setOrigin(btVector3(pos.x + config->position.x() * modelScale.x, pos.y + config->position.y() * modelScale.y, pos.z + config->position.z() * modelScale.z));
		btGhostObject *btGhost = NULL;
		btDefaultMotionState *myMotionState = NULL;
		btRigidBody *btBody = NULL;
		if (config->isGhost && i == 0) {
			btGhost = new btGhostObject();
			btGhost->setUserPointer(this);
			btGhost->setCollisionShape(config->shape);
			btGhost->setWorldTransform(startTransform);
			btGhost->setCollisionFlags(btGhost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			context->world->btWorld->addCollisionObject(btGhost, config->collisionGroup, config->collisionMask);
			btBody = NULL;
		} else if (!config->isGhost) {
			myMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(config->mass, myMotionState, config->shape, localInertia);
			rbInfo.m_friction = config->friction;
			btBody = new btRigidBody(rbInfo);
			btBody->setUserPointer(this);
			btBody->setAngularFactor(config->angularFactor);
			btBody->setActivationState(config->activationState);
			if (config->collisionFlags) {
				btBody->setCollisionFlags(btBody->getCollisionFlags() | config->collisionFlags);
			}
			context->world->btWorld->addRigidBody(btBody, config->collisionGroup, config->collisionMask);
			btBody->setActivationState(config->activationState);
			if (config->customGravitySet) {
				btBody->setGravity(config->gravity);
			}
			btBody->setDamping(config->linearDamping, config->angularDamping);
			btGhost = NULL;
		} else {
			logmsg("Error configuring object physics - ghost only supported on physics configuration 0");
		}
		if (i == 0) {
			this->myMotionState = myMotionState;
			this->btBody = btBody;
			this->btGhost = btGhost;
		} else {
			extraBodies->add(btBody);
			extraMotionStates->add(myMotionState);
		}
	}
	*/
	isInitialized = TRUE;
}

void GameObject::update() {
	if (!isInitialized) {
		return;
	}
	if (luaBinder) {
		luaBinder->update();
	}
	/*
	btTransform transform;
	if (btBody) {
		transform = btBody->getWorldTransform();
		if (btBody->getActivationState() == DISABLE_SIMULATION) {
			logmsg("Simulation has been disabled for this object!");
		}
	} else if (btGhost) {
		transform = btGhost->getWorldTransform();
	} else {
		transform = transform.getIdentity();
	}
	// subtract the model offset
	PhysicsModelConfig *config = physicsModelConfigs->array[0];
	btVector3 modelOffset = transform.getBasis() * btVector3(config->position.x() * modelScale.x, config->position.y() * modelScale.y, config->position.z() * modelScale.z);
	btVector3 origin = transform.getOrigin() - modelOffset;
	pos = Vector3f(origin.x(), origin.y(), origin.z());
	Vector3f posDelta = pos - lastPos;
	lastPos = pos;
	// update any other gameobjects paths with our position delta
	World *world = context->world;
	GameObject **objs = world->gameObjects->array;
	S32 objCount = world->gameObjects->getSize();
	// this shouldn't be too bad on CPU unless there are tons of objects with really long paths
	for (S32 i = 0; i < objCount; i++) {
		GameObject *other = objs[i];
		if (other->nextPathSeek > -1) {
			for (S32 j = other->nextPathSeek; j < other->pathLength; j++) {
				// having us check for our own presence on another pathnode makes this safe to delete objects because a deleted object will never be able to check itself.
				if (other->path[j].attachment == this) {
					other->path[j].pos += posDelta;
				}
			}
		}
	}
	//char buf[255];
	//sprintf(buf, "Activation State %d %f %f %f", btBody->getActivationState(), vel.x(), vel.y(), vel.z());
	//logmsg(buf);
	updatePathing();
	*/
}

/*
void GameObject::updatePathing() {
	if (!btBody) return;
	if (nextPathSeek > -1) {
		Vector3f seekPoint = path[nextPathSeek].pos;
		F32 distance = (seekPoint - Vector3f(pos.x,pos.y,seekPoint.z)).lengthSq();
		if (distance < 2) {
			nextPathSeek++;
			if (nextPathSeek == pathLength) {
				nextPathSeek = -1;
				pathLength = 0;
				btBody->setLinearVelocity(btVector3(0,0,0));
			} else {
				seekPoint = path[nextPathSeek].pos;
			}
		}
		if (nextPathSeek > -1) {
			Vector3f dir = seekPoint - pos;
			dir.z = 0;
			dir.normalize();
			F32 targetZRot = atan2(dir.y, dir.x) - HALFPI;
			F32 rotDelta = targetZRot - zRotation;
			if (rotDelta > PI) {
				rotDelta -= TAU;
			} else if (rotDelta < -PI) {
				rotDelta += TAU;
			}
			F32 rotAmt;
			if (rotDelta < -GAMEOBJECT_SMALL_ROTATION_THRESHOLD || rotDelta > GAMEOBJECT_SMALL_ROTATION_THRESHOLD) {
				rotAmt = largeRotationalSpeed * context->tickDelta;
			} else {
				rotAmt = smallRotationalSpeed * context->tickDelta;
			}
			if (rotDelta > rotAmt) {
				rotDelta = rotAmt;
			} else if (rotDelta < -rotAmt) {
				rotDelta = -rotAmt;
			}
			zRotation += rotDelta;
			if (zRotation > PI) {
				zRotation -= TAU;
			} else if (zRotation < -PI) {
				zRotation += TAU;
			}
			//btBody->getWorldTransform().getBasis().setEulerZYX(0, 0, atan2(dir.y, dir.x) - HALFPI);
			btBody->getWorldTransform().getBasis().setEulerZYX(0, 0, zRotation);
			btBody->setAngularVelocity(btVector3(0,0,0));
			btVector3 vel = btBody->getLinearVelocity();
			btVector3 btDir(dir.x,dir.y,dir.z);
			Vector3f xyVelocity = Vector3f(vel.x(), vel.y(), 0);
			btBody->setLinearVelocity((btDir * pathingSpeedClamp) + btVector3(0, 0, vel.z()));
			// TODO - don't clamp if dir is pushing in opposite direction or if we can clamp after impulse applied - that would be better.
			if (xyVelocity.length() < pathingSpeedClamp) {
				//btBody->applyCentralImpulse(btDir * pathingAccel * context->tickDelta);
			}
			btBody->activate(true);
		}
	} else {
		if (pathLength > 0) {
			nextPathSeek = 0;
		}
	}
}
 */

void GameObject::destroyBody() {
#ifdef BATTERYTECH_INCLUDE_BULLET
	if (constraints) {
		for (S32 i = 0; i < constraints->getSize(); i++) {
			context->world->btWorld->removeConstraint(constraints->array[i]);
		}
		constraints->deleteElements();
	}
	if (btBody) {
		context->world->btWorld->removeRigidBody(btBody);
		delete btBody;
		delete myMotionState;
		btBody = NULL;
		myMotionState = NULL;
	} else if (btGhost) {
		context->world->btWorld->removeCollisionObject(btGhost);
		delete btGhost;
		btGhost = NULL;
	}
	if (extraBodies) {
		for (S32 i = 0; i < extraBodies->getSize(); i++) {
			context->world->btWorld->removeRigidBody(extraBodies->array[i]);
		}
		extraBodies->deleteElements();
		delete extraBodies;
		extraMotionStates->deleteElements();
		delete extraMotionStates;
		extraBodies = NULL;
		extraMotionStates = NULL;
	}
#endif
#ifdef BATTERYTECH_INCLUDE_BOX2D
    if (boxBody && context && context->world && context->world->boxWorld) {
		//logmsg("b2World->DestroyBody");
		context->world->boxWorld->DestroyBody(boxBody);
		boxBody = NULL;
	}
#endif
}

World* GameObject::getWorld() {
	return context->world;
}

/*
void GameObject::startCollisionPhase() {
	// swap contacts into oldContacts
	ManagedArray<GameObject> *c = oldContacts;
	oldContacts = contacts;
	contacts = c;
	// clear contacts
	contacts->clear();
	wasLevelColliding = isLevelColliding;
	isLevelColliding = FALSE;
}

void GameObject::onCollision(GameObject *other) {
	contacts->add(other);
}

void GameObject::onLevelCollision() {
	isLevelColliding = TRUE;
}

void GameObject::endCollisionPhase() {
	// compare contacts with oldContacts and dispatch as necessary
	// first dispath lost contacts
	// TODO - The two-way checks here would be much faster with a hash!  Switch to iterating hashtables
	for (S32 i = 0; i < oldContacts->getSize(); i++) {
		GameObject *contact = oldContacts->array[i];
		if (isValidGameObject(contact) && contact->isActive) {
			BOOL32 retained = FALSE;
			for (S32 j = 0; j < contacts->getSize(); j++) {
				if (contacts->array[j] == contact) {
					retained = TRUE;
					break;
				}
			}
			if (!retained) {
				luaBinder->onCollisionEnded(contact);
			}
		}
	}
	// now dispatch new contacts
	for (S32 i = 0; i < contacts->getSize(); i++) {
		GameObject *contact = contacts->array[i];
		BOOL32 newContact = TRUE;
		for (S32 j = 0; j < oldContacts->getSize(); j++) {
			if (oldContacts->array[j] == contact) {
				newContact = FALSE;
				break;
			}
		}
		if (newContact) {
			luaBinder->onCollision(contact);
		}
	}
	if (!wasLevelColliding && isLevelColliding) {
		luaBinder->onLevelCollision();
	} else if (wasLevelColliding && !isLevelColliding) {
		luaBinder->onLevelCollisionEnded();
	}
}

*/

BOOL32 GameObject::isValidGameObject(GameObject *other) {
	ManagedArray<GameObject> *objs = context->world->gameObjects;
	// TODO - would perform MUCH better with a hash!
	for (S32 i = 0; i < objs->getSize(); i++) {
		if (other == objs->array[i]) {
			return TRUE;
		}
	}
	return FALSE;
}
