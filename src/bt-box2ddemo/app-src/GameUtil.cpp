/*
 * GameUtil.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rgreen
 */

#include "GameUtil.h"

#include <bt-box2d/Collision/b2Collision.h>
#include "StdQueryCallback.h"
#include <batterytech/render/GraphicsConfiguration.h>

GameUtil::GameUtil() {
}

GameUtil::~GameUtil() {
}

F32 GameUtil::screenToWorldX(S32 screenX, GameContext *context) {
	return WORLD_LEFT + (screenX / (F32)context->gConfig->viewportWidth) * (WORLD_RIGHT - WORLD_LEFT);
}

F32 GameUtil::screenToWorldY(S32 screenY, GameContext *context) {
	return WORLD_TOP + (screenY / (F32)context->gConfig->viewportHeight) * (WORLD_BOTTOM - WORLD_TOP);
}

S32 GameUtil::worldToScreenX(F32 worldX, GameContext *context) {
	return (S32)(((worldX - WORLD_LEFT) / (WORLD_RIGHT - WORLD_LEFT)) * (F32)context->gConfig->viewportWidth);
}

S32 GameUtil::worldToScreenY(F32 worldY, GameContext *context) {
	return (S32)(((worldY - WORLD_TOP) / (WORLD_BOTTOM - WORLD_TOP)) * (F32)context->gConfig->viewportHeight);
}

GameObject* GameUtil::findObjectIntersection(F32 worldX, F32 worldY, World *world) {
	// use boxworld AABB query to select
	b2AABB aabb;
	b2Vec2 d = b2Vec2(0.001f, 0.001f);
	b2Vec2 mouse = b2Vec2(worldX, worldY);
	aabb.lowerBound = mouse - d;
	aabb.upperBound = mouse + d;
	// Query the world for overlapping shapes.
	StdQueryCallback callback(mouse);
	world->boxWorld->QueryAABB(&callback, aabb);
	if (callback.m_fixture) {
		if (callback.m_fixture->GetBody()->GetUserData()) {
			PhysicsBodyObject *obj = (PhysicsBodyObject*) callback.m_fixture->GetBody()->GetUserData();
			if (obj->bodyType == PHYSICS_BODY_TYPE_GAMEOBJECT) {
				return (GameObject*) obj;
			}
		}
	}
	return NULL;
}

