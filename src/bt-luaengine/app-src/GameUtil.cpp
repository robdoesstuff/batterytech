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

#include "GameUtil.h"

#include "GameContext.h"
#include <batterytech/render/GraphicsConfiguration.h>
#include "World.h"
#include "render/Camera.h"
#include "gameobject/GameObject.h"
//#include <LinearMath/btAabbUtil2.h>
#include <batterytech/platform/opengles.h>
#include "level/Level.h"
#include <batterytech/Logger.h>

using namespace BatteryTech;

// static field init
GameObject** GameUtil::pickArray = new GameObject*[MAX_PICKOBJECTS];

GameUtil::GameUtil() {
}

GameUtil::~GameUtil() {
}

Vector3f GameUtil::getScreenCoord(const Vector3f &worldCoord, GameContext *context) {
	Vector3f v = gles_gluProject(worldCoord, context->world->camera->matrix, context->world->camera->proj, Vector2f(0,0), Vector2f(context->gConfig->viewportWidth, context->gConfig->viewportHeight));
	return Vector3f(v.x, context->gConfig->viewportHeight - v.y, v.z);
}

/*GameObject* GameUtil::pickObject(const Vector2f &screenCoord, Context *context, F32 xySlop) {
	Vector3f pos = context->world->camera->pos;
	Vector3f dir = context->world->camera->castRayFromViewport(screenCoord.x, screenCoord.y);
	//char buf[100];
	//sprintf(buf, "Click ray point is {%f, %f, %f} dir is {%f, %f, %f}", pos.x, pos.y, pos.z, dir.x, dir.y, dir.z);
	//logmsg(buf);
	btVector3 min,max;
	btVector3 rayFrom(pos.x,pos.y,pos.z);
	btVector3 rayTo(dir.x * 1000, dir.y * 1000, dir.z * 1000);
	btScalar hitLambda = 1.0f;
	btVector3 hitNormal;
	GameObject *closestObj = NULL;
	btScalar closestLambda = 1000.0f;
	for (S32 j = 0; j < context->world->gameObjects->getSize(); j++) {
		GameObject *obj = context->world->gameObjects->array[j];
		if (obj->btBody) {
			obj->btBody->getAabb(min, max);
		} else if (obj->btGhost) {
			obj->btGhost->getCollisionShape()->getAabb(obj->btGhost->getWorldTransform(),min,max);
		}
		min -= btVector3(xySlop, xySlop, 0);
		max += btVector3(xySlop, xySlop, 0);
		if (btRayAabb(rayFrom, rayTo, min, max, hitLambda, hitNormal)) {
			if (hitLambda < closestLambda) {
				closestObj = obj;
				closestLambda = hitLambda;
			}
		}
	}
	return closestObj;
}*/

/*GameObject** GameUtil::pickObjects(const Vector2f &screenCoord, Context *context, F32 xySlop, S32 *count) {
	Vector3f pos = context->world->camera->pos;
	Vector3f dir = context->world->camera->castRayFromViewport(screenCoord.x, screenCoord.y);
	//char buf[100];
	//sprintf(buf, "Click ray point is {%f, %f, %f} dir is {%f, %f, %f}", pos.x, pos.y, pos.z, dir.x, dir.y, dir.z);
	//logmsg(buf);
	btVector3 min,max;
	btVector3 rayFrom(pos.x,pos.y,pos.z);
	btVector3 rayTo(dir.x * 1000, dir.y * 1000, dir.z * 1000);
	btScalar lambda = 1.0f;
	btVector3 hitNormal;
	S32 hitCount = 0;
	F32 lambdas[MAX_PICKOBJECTS];
	for (S32 i = 0; i < MAX_PICKOBJECTS; i++) {
		lambdas[i] = 0;
	}
	for (S32 i = 0; i < context->world->gameObjects->getSize(); i++) {
		GameObject *obj = context->world->gameObjects->array[i];
		if (obj->btBody) {
			obj->btBody->getAabb(min, max);
		} else if (obj->btGhost) {
			obj->btGhost->getCollisionShape()->getAabb(obj->btGhost->getWorldTransform(),min,max);
		}
		min -= btVector3(xySlop, xySlop, 0);
		max += btVector3(xySlop, xySlop, 0);
		lambda = 1.0f;
		if (btRayAabb(rayFrom, rayTo, min, max, lambda, hitNormal)) {
			hitCount++;
			if (hitCount > MAX_PICKOBJECTS) {
				hitCount = MAX_PICKOBJECTS;
			}
			BOOL32 inserted = FALSE;
			// test against existing objects in array
			for (S32 j = 0; j < hitCount-1; j++) {
				// if this object is closer than the existing entry
				if (lambda < lambdas[j]) {
					//shift all others back one
					for (S32 k = hitCount-1; k > j; k--) {
						if (k < MAX_PICKOBJECTS) {
							// this throws out the furthest objects if they don't fit in the arrays
							pickArray[k] = pickArray[k-1];
							lambdas[k] = lambdas[k-1];
						}
					}
					//now insert at j
					pickArray[j] = obj;
					lambdas[j] = lambda;
					inserted = TRUE;
				}
			}
			// only insert if not already inserted and there is an open spot at the end of the array
			// (Because this is further back than any existing element or there are no elements).
			if (!inserted && hitCount < MAX_PICKOBJECTS) {
				pickArray[hitCount-1] = obj;
				lambdas[hitCount-1] = lambda;
			}
		}
	}
	*count = hitCount;
	return pickArray;
}*/

/*Vector3f GameUtil::castRayToLevel(const Vector2f &screenCoord, Context *context) {
	Level *level = context->world->level;
	Vector3f pos = context->world->camera->pos;
	Vector3f dir = context->world->camera->castRayFromViewport(screenCoord.x, screenCoord.y);
	btVector3 rayFrom(pos.x,pos.y,pos.z);
	btVector3 rayTo(dir.x * 1000, dir.y * 1000, dir.z * 1000);
	btTransform rayFromTrans;
	btTransform rayToTrans;
	rayFromTrans.setIdentity();
	rayToTrans.setIdentity();
	rayFromTrans.setOrigin(rayFrom);
	rayToTrans.setOrigin(rayTo);
	btCollisionWorld::ClosestRayResultCallback callback(rayFrom, rayTo);
	//btCollisionWorld::RayResultCallback
	btCollisionWorld::rayTestSingle(rayFromTrans, rayToTrans, level->btBody, level->shape, level->btBody->getWorldTransform(), callback);
	if (callback.hasHit()) {
		// logmsg("Hit!");
		// TODO - also deliver the normal (I think I'll need it.)
		return Vector3f(callback.m_hitPointWorld.x(), callback.m_hitPointWorld.y(), callback.m_hitPointWorld.z());
	}
	return Vector3f(0,0,0);
}*/

/*Vector3f GameUtil::castRayToGameObject(const Vector2f &screenCoord, GameObject *gameObject, Context *context) {
	Vector3f pos = context->world->camera->pos;
	Vector3f dir = context->world->camera->castRayFromViewport(screenCoord.x, screenCoord.y);
	btVector3 rayFrom(pos.x,pos.y,pos.z);
	btVector3 rayTo(dir.x * 1000, dir.y * 1000, dir.z * 1000);
	btTransform rayFromTrans;
	btTransform rayToTrans;
	rayFromTrans.setIdentity();
	rayToTrans.setIdentity();
	rayFromTrans.setOrigin(rayFrom);
	rayToTrans.setOrigin(rayTo);
	btCollisionWorld::ClosestRayResultCallback callback(rayFrom, rayTo);
	//btCollisionWorld::RayResultCallback
	if (gameObject->btBody) {
		btCollisionWorld::rayTestSingle(rayFromTrans, rayToTrans, gameObject->btBody, gameObject->physicsModelConfigs->array[0]->shape, gameObject->btBody->getWorldTransform(), callback);
	} else if (gameObject->btGhost) {
		btCollisionWorld::rayTestSingle(rayFromTrans, rayToTrans, gameObject->btGhost, gameObject->physicsModelConfigs->array[0]->shape, gameObject->btGhost->getWorldTransform(), callback);
	}
	if (callback.hasHit()) {
		// logmsg("Hit!");
		// TODO - also deliver the normal (I think I'll need it.)
		return Vector3f(callback.m_hitPointWorld.x(), callback.m_hitPointWorld.y(), callback.m_hitPointWorld.z());
	}
	return Vector3f(0,0,0);
}*/

/*F32 GameUtil::getLineOfSightDistance(const Vector3f &from, const Vector3f &to, Context *context, GameObject *exclusion1, GameObject *exclusion2) {
	// check level first
	Level *level = context->world->level;
	btVector3 rayFrom(from.x,from.y,from.z);
	btVector3 rayTo(to.x, to.y, to.z);
	btTransform rayFromTrans;
	btTransform rayToTrans;
	rayFromTrans.setIdentity();
	rayToTrans.setIdentity();
	rayFromTrans.setOrigin(rayFrom);
	rayToTrans.setOrigin(rayTo);
	btCollisionWorld::ClosestRayResultCallback callback(rayFrom, rayTo);
	btCollisionWorld::rayTestSingle(rayFromTrans, rayToTrans, level->btBody, level->shape, level->btBody->getWorldTransform(), callback);
	if (callback.hasHit()) {
		return (rayFrom - callback.m_hitPointWorld).length();
	}
	// now check game objects
	btScalar hitLambda;
	btVector3 hitNormal;
	btVector3 min,max;
	btScalar furthestLOS = -1;
	// TODO - broadphase with distsq?
	for (S32 j = 0; j < context->world->gameObjects->getSize(); j++) {
		GameObject *obj = context->world->gameObjects->array[j];
		// exclusions from test
		if (obj != exclusion1 && obj != exclusion2 && obj->btBody) {
			obj->btBody->getAabb(min, max);
			if (btRayAabb(rayFrom, rayTo, min, max, hitLambda, hitNormal)) {
				if (hitLambda > furthestLOS) {
					furthestLOS = hitLambda;
				}
			}
		}
	}
	if (furthestLOS < 0) {
		return (rayFrom - rayTo).length();
	}
	return furthestLOS;
}*/
