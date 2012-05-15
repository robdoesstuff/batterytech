/*
 * GameObject.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: rgreen
 */

#include "GameObject.h"
#include <bt-box2d/Dynamics/Contacts/b2Contact.h>
#include <string.h>
#include "GameObjectMeta.h"
#include "GameObjectPropertyMeta.h"
#include <batterytech/Logger.h>

GameObject::GameObject(GameContext *context) : PhysicsBodyObject(PHYSICS_BODY_TYPE_GAMEOBJECT) {
	this->context = context;
	type = GAMEOBJECT_TYPE_NONE;
	isActive = TRUE;
	x = 0;
	y = 0;
	z = 0;
	angle = 0;
	boxBody = NULL;
	processContact = FALSE;
	preSolveVelocity = 0;
	postSolveVelocity = 0;
	impactVelocityDelta = 0;
	userId = GAMEOBJECT_NO_USER_ID;
	userGroupId = GAMEOBJECT_NO_USER_GROUP_ID;
	properties = new ManagedArray<GameObjectProperty>(10);
}

GameObject::~GameObject() {
	boxBody = NULL;
	if (properties) {
		properties->deleteElements();
		delete properties;
	}
	properties = NULL;
}

void GameObject::destroyBody() {
	if (boxBody && context && context->world && context->world->boxWorld) {
		//logmsg("b2World->DestroyBody");
		context->world->boxWorld->DestroyBody(boxBody);
		boxBody = NULL;
	}
}

World* GameObject::getWorld() {
	return context->world;
}

// not weighting for volume, just centering on min/max
Vector2f GameObject::calcCenter(Vector2f *points, S32 count) {
	F32 minX = 99999999.9f;
	F32 minY = 99999999.9f;
	F32 maxX = -99999999.9f;
	F32 maxY = -99999999.9f;
	S32 i;
	for (i = 0; i < count; i++) {
		if (points[i].x < minX) {
			minX = points[i].x;
		}
		if (points[i].x > maxX) {
			maxX = points[i].x;
		}
		if (points[i].y < minY) {
			minY = points[i].y;
		}
		if (points[i].y > maxY) {
			maxY = points[i].y;
		}
	}
	return Vector2f(minX + (maxX - minX) / 2, minY + (maxY - minY) / 2);
}

void GameObject::contactStarted(b2Contact* contact) {
}

void GameObject::contactEnded(b2Contact* contact) {
}

void GameObject::contactPreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
	::PhysicsBodyObject *objA = (::PhysicsBodyObject*) contact->GetFixtureA()->GetBody()->GetUserData();
	::PhysicsBodyObject *objB = (::PhysicsBodyObject*) contact->GetFixtureB()->GetBody()->GetUserData();
	if (objA->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
		GameObject *gObjA = (GameObject*) objA;
		if (gObjA->type == GAMEOBJECT_TYPE_BALL) {
			preSolveVelocity = gObjA->getLinearVelocity();
		}
	}
	if (objB->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
		GameObject *gObjB = (GameObject*) objB;
		if (gObjB->type == GAMEOBJECT_TYPE_BALL) {
			preSolveVelocity = gObjB->getLinearVelocity();
		}
	}
}

void GameObject::contactPostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
	::PhysicsBodyObject *objA = (::PhysicsBodyObject*) contact->GetFixtureA()->GetBody()->GetUserData();
	::PhysicsBodyObject *objB = (::PhysicsBodyObject*) contact->GetFixtureB()->GetBody()->GetUserData();
	if (objA->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
		GameObject *gObjA = (GameObject*) objA;
		if (gObjA->type == GAMEOBJECT_TYPE_BALL) {
			postSolveVelocity = gObjA->getLinearVelocity();
			impactVelocityDelta += preSolveVelocity - postSolveVelocity;
			processContact = TRUE;
		}
	}
	if (objB->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
		GameObject *gObjB = (GameObject*) objB;
		if (gObjB->type == GAMEOBJECT_TYPE_BALL) {
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

void GameObject::sendEvent(const char* eventName) {
}

void GameObject::setProperty(const char *name, const char *value) {
	//update existing property, otherwise create new one
	S32 i;
	for (i = 0; i < properties->getSize(); i++) {
		if (strcmp(properties->array[i]->name, name) == 0) {
			properties->array[i]->setValue(value);
			return;
		}
	}
	properties->add(new GameObjectProperty(name, value));
}

ManagedArray<GameObjectProperty>* GameObject::getProperties() {
	return properties;
}

const char* GameObject::getProperty(const char *name) {
	if (properties) {
		for (S32 i = 0; i < properties->getSize(); i++) {
			if (strcmp(properties->array[i]->name, name) == 0) {
				return properties->array[i]->value;
			}
		}
	}
	return NULL;
}

void GameObject::loadPropertyFrom(char* line) {
	if (strstr(line, "id ") == &line[0]) {
		sscanf(line, "id %d", &userId);
	}
	if (strstr(line, "group ") == &line[0]) {
		sscanf(line, "group %d", &userGroupId);
	}
	GameObjectMeta *meta = getIMeta();
	ManagedArray<GameObjectPropertyMeta> *propsMeta = meta->getProperties();
	S32 i;
	char propName[255];
	char propValue[255];
	// use properties meta to match property names and values
	for (i = 0; i < propsMeta->getSize(); i++) {
		GameObjectPropertyMeta *propMeta = propsMeta->array[i];
		// add a space to the name to make for more accurate matching
		sprintf(propName, "%s ", propMeta->name);
		if (strstr(line, propName) == &line[0]) {
			// now add the string field to the name so we can scan in a value
			strcat(propName, " %s");
			sscanf(line, propName, &propValue);
			// use the clean name from the meta to set the name/value
			setProperty(propMeta->name, propValue);
		}
	}
}

void GameObject::storePropertiesTo(FILE *file) {
	char buf[255];
	if (userId != GAMEOBJECT_NO_USER_ID) {
		sprintf(buf, "id %d\n", userId);
		fputs(buf, file);
	}
	if (userGroupId != GAMEOBJECT_NO_USER_GROUP_ID) {
		sprintf(buf, "group %d\n", userGroupId);
		fputs(buf, file);
	}
	S32 i;
	for (i = 0; i < properties->getSize(); i++) {
		sprintf(buf, "%s %s\n", properties->array[i]->name, properties->array[i]->value);
		fputs(buf, file);
	}
}
