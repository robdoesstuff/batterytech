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
 	boxBodies = new ManagedArray<b2Body>(GAMEOBJECT_MAX_EXTRA_BODIES);
 	for (S32 i = 0; i < GAMEOBJECT_MAX_EXTRA_BODIES; i++) {
 		boxBodies->array[i] = NULL;
 	}
 	boxFixtures = new HashTable<S32, b2Fixture*>(GAMEOBJECT_MAX_FIXTURES * 1.3f);
    callbackDetail = CALLBACK_DETAIL_NONE;
    contacts = new ManagedArray<PhysicsContact2D>(GAMEOBJECT_MAX_CONTACTS);
    for (S32 i = 0; i < GAMEOBJECT_MAX_CONTACTS; i++) {
        contacts->add(new PhysicsContact2D);
    }
    contactObjects = new HashTable<GameObject*, GameObject*>(GAMEOBJECT_MAX_CONTACTS * 1.5f);
    contactsUsed = 0;
    nextFixtureId = 0;
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
#ifdef BATTERYTECH_INCLUDE_BOX2D
	destroyBody();
	delete boxBodies;
	boxBodies = NULL;
	contacts->deleteElements();
	delete contacts;
	contacts = NULL;
	delete contactObjects;
	contactObjects = NULL;
	delete boxFixtures;
	boxFixtures = NULL;
#endif
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
GameObject::PhysicsContact2D* GameObject::findContact(b2Contact* contact) {
    GameObject *other = NULL;
    b2Fixture *f = NULL;
    ::PhysicsBodyObject *objA = (::PhysicsBodyObject*) contact->GetFixtureA()->GetBody()->GetUserData();
    ::PhysicsBodyObject *objB = (::PhysicsBodyObject*) contact->GetFixtureB()->GetBody()->GetUserData();
    if (objA->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT && objB->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
        GameObject *gObjA = (GameObject*) objA;
        GameObject *gObjB = (GameObject*) objB;
        if (gObjA == this) {
            other = gObjB;
            f = contact->GetFixtureB();
        } else if (gObjB == this) {
            other = gObjA;
            f = contact->GetFixtureA();
        }
    }
    if (other && f) {
        for (S32 i = 0; i < contactsUsed; i++) {
            PhysicsContact2D *pc = contacts->array[i];
            if (pc->other == other && pc->fixture == f) {
                return pc;
            }
        }
    }
    return NULL;
}

void GameObject::contactStarted(b2Contact* contact) {
	if (callbackDetail == CALLBACK_DETAIL_NONE) {
		return;
	}
    if (contactsUsed == GAMEOBJECT_MAX_CONTACTS) {
        // error - no more contacts available
        logmsg("Error - no more contacts available for GameObject.  Increase max or simplify collisions!");
        return;
    }
    // add to list
    GameObject *other = NULL;
    b2Fixture *f = NULL;
    ::PhysicsBodyObject *objA = (::PhysicsBodyObject*) contact->GetFixtureA()->GetBody()->GetUserData();
    ::PhysicsBodyObject *objB = (::PhysicsBodyObject*) contact->GetFixtureB()->GetBody()->GetUserData();
    if (objA->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT && objB->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
        GameObject *gObjA = (GameObject*) objA;
        GameObject *gObjB = (GameObject*) objB;
        if (gObjA == this) {
            other = gObjB;
            f = contact->GetFixtureB();
        } else if (gObjB == this) {
            other = gObjA;
            f = contact->GetFixtureA();
        }
    }
    if (!other || !f) {
        logmsg("Error - contact is not of gameobject type");
        return;
    }
    PhysicsContact2D *pc = contacts->array[contactsUsed++];
    pc->isActive = TRUE;
    pc->wasTouching = FALSE;
    pc->isTouching = contact->IsTouching();
    pc->other = other;
    pc->fixture = f;
    pc->callbackProcessed = FALSE;
    pc->approachVelocity = 0;
    pc->impulse = 0;
    b2Manifold *manifold = contact->GetManifold();
    for (S32 i = 0; i < manifold->pointCount; i++) {
        pc->localPoint[i] = Vector2f(manifold->points[i].localPoint.x, manifold->points[i].localPoint.y);
    }
    pc->pointCount = manifold->pointCount;
}

void GameObject::contactEnded(b2Contact* contact) {
	if (callbackDetail == CALLBACK_DETAIL_NONE) {
		return;
	}
    // broad
    // remove from list
    PhysicsContact2D *pc = findContact(contact);
    if (pc) {
        pc->isActive = FALSE;
        pc->isTouching = FALSE;
        pc->callbackProcessed = FALSE;
    }
}

void GameObject::contactPreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2PointState state1[2], state2[2];
    b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());
    if (state2[0] == b2_addState) {
        PhysicsContact2D *pc = findContact(contact);
        if (pc) {
            const b2Body* bodyA = contact->GetFixtureA()->GetBody();
            const b2Body* bodyB = contact->GetFixtureB()->GetBody();
            b2Vec2 point = worldManifold.points[0];
            b2Vec2 vA = bodyA->GetLinearVelocityFromWorldPoint(point);
            b2Vec2 vB = bodyB->GetLinearVelocityFromWorldPoint(point);
            F32 approachVelocity = b2Dot(vB - vA, worldManifold.normal);
            pc->approachVelocity += approachVelocity;
        }
    }
}

void GameObject::contactPostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
	if (callbackDetail == CALLBACK_DETAIL_NONE) {
		return;
	}
	// gather impulse data and update contact point locations
	PhysicsContact2D *pc = findContact(contact);
    if (!pc) {
        return;
    }
	pc->impulse += impulse->normalImpulses[0];
    b2Manifold *manifold = contact->GetManifold();
    pc->pointCount = manifold->pointCount;
	if (pc->pointCount > 1) {
		pc->impulse += impulse->normalImpulses[1];
	}
    for (S32 i = 0; i < manifold->pointCount; i++) {
        pc->localPoint[i] = Vector2f(manifold->points[i].localPoint.x, manifold->points[i].localPoint.y);
    }
	if (callbackDetail != CALLBACK_DETAIL_NARROW) {
		return;
	}
	// only set it to not processed for narrow collision callbacks
	pc->callbackProcessed = FALSE;
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

#ifdef BATTERYTECH_INCLUDE_BOX2D
	for (S32 i = 0; i < boxBodies->getSize(); i++) {
		// use position of first box body
		if (boxBodies->array[i] != NULL) {
			b2Vec2 pos = boxBodies->array[i]->GetPosition();
			this->pos.x = pos.x;
			this->pos.y = pos.y;
		}
	}
	if (callbackDetail != CALLBACK_DETAIL_NONE) {
		for (S32 i = 0; i < contactsUsed; i++) {
			PhysicsContact2D *pc = contacts->array[i];
			if (!pc->callbackProcessed && pc->other) {
				BOOL32 isNew = (contactObjects->get(pc->other) == NULL);
				if (!pc->wasTouching && pc->isTouching) {
					if (isNew) {
						contactObjects->put(pc->other, pc->other);
						luaBinder->onCollisionStarted(pc->other, pc->impulse, pc->approachVelocity);
					}
					pc->wasTouching = TRUE;
				} else if (pc->wasTouching && !pc->isTouching) {
					// are there any other contacts still alive?  If not, notify script that contact has ended
					BOOL32 otherContactsExist = FALSE;
					for (S32 j = 0; j < contactsUsed; j++) {
						PhysicsContact2D *otherPC = contacts->array[j];
						if (otherPC != pc && otherPC->other == pc->other) {
							otherContactsExist = TRUE;
							break;
						}
					}
					if (!otherContactsExist) {
						luaBinder->onCollisionEnded(pc->other);
						contactObjects->remove(pc->other);
					}
					pc->wasTouching = FALSE;
				} else {
					if (callbackDetail == CALLBACK_DETAIL_NARROW) {
						luaBinder->onCollisionUpdated(pc->other, pc->impulse, pc->approachVelocity);
					}
				}
				pc->impulse = 0;
                pc->approachVelocity = 0;
				pc->callbackProcessed = TRUE;
			}
			if (!pc->isActive) {
				// swap with last
				contacts->array[i] = contacts->array[contactsUsed-1];
				contacts->array[contactsUsed-1] = pc;
				contactsUsed--;
				i--;
			}
		}
	}
#endif

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
    if (context && context->world && context->world->boxWorld && boxBodies) {
		for (S32 i = 0; i < boxBodies->getSize(); i++) {
			if (boxBodies->array[i]) {
				context->world->boxWorld->DestroyBody(boxBodies->array[i]);
			}
		}
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

void GameObject::onOtherGameObjectRemoved(GameObject *other) {
#ifdef BATTERYTECH_INCLUDE_BOX2D
    if (callbackDetail != CALLBACK_DETAIL_NONE) {
        contactObjects->remove(other);
		for (S32 i = 0; i < contactsUsed; i++) {
			PhysicsContact2D *pc = contacts->array[i];
            if (pc->other == other) {
                pc->other = NULL;
                pc->isActive = FALSE;
            }
        }
    }
#endif
}
