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

#include "Level.h"
#include "../GameContext.h"
#include <batterytech/primitives.h>
#include <batterytech/importers/obj/ObjImporter.h>
#include <batterytech/platform/platformgeneral.h>
#include "../World.h"
#include <batterytech/Logger.h>
#include "../Game.h"

using namespace BatteryTech;

Level::Level(GameContext *context) : PhysicsBodyObject(PHYSICS_BODY_TYPE_STATIC_GEOMETRY) {
	 this->context = context;
	 //btBody = NULL;
	 //shape = NULL;
	 //groundMesh = NULL;
	 scene = NULL;
}

Level::~Level() {
	context = NULL;
	//delete groundMesh;
	//groundMesh = NULL;
	//delete shape;
	//shape = NULL;
	delete scene;
	scene = NULL;
}

void Level::load(const char *assetName) {
	//if (groundMesh) {
	//	delete groundMesh;
	//}
	//if (shape) {
	//	delete shape;
	//}
	if (scene) {
		delete scene;
	}
	//groundMesh = new btTriangleMesh(true, false);
	//ObjImporter importer;
	//F32 scale = 1.0f;
	char *text = _platform_load_text_asset(assetName);
	if (!text) {
		context->game->showError("Level File Not Found", assetName);
		return;
	}
	/*scene = importer.import(text);
	const char *error = importer.getError();
	if (error) {
		logmsg(error);
	}
	_platform_free_asset((unsigned char*) text);*/
	/*ObjGroup *collisionMesh = NULL;
	char *collisionGroupName = context->game->luaBinder->getString("levelCollisionGroup");
	if (collisionGroupName) {
		collisionMesh = scene->getGroup(collisionGroupName);
	}
	delete [] collisionGroupName;
	if (!collisionMesh) {
		logmsg("** No Collision Mesh Found - Using Drawable Geometry - Performance May Suffer **");
		F32 *verts = scene->vertPositions;
		for (S32 i = 0; i < scene->vertCount / 3; i++) {
			groundMesh->addTriangle(btVector3(verts[i * 9] * scale, verts[i * 9 + 1] * scale, verts[i * 9 + 2] * scale),
					btVector3(verts[i * 9 + 3] * scale, verts[i * 9 + 4] * scale, verts[i * 9 + 5] * scale),
					btVector3(verts[i * 9 + 6] * scale, verts[i * 9 + 7] * scale, verts[i * 9 + 8] * scale),
					true);
		}
	} else {
		for (S32 meshIdx = 0; meshIdx < collisionMesh->meshCount; meshIdx++) {
			ObjMesh *mesh = &collisionMesh->meshes[meshIdx];
			F32 *verts = mesh->vertPositions;
			for (S32 i = 0; i < mesh->vertCount / 3; i++) {
				groundMesh->addTriangle(btVector3(verts[i * 9] * scale, verts[i * 9 + 1] * scale, verts[i * 9 + 2] * scale),
						btVector3(verts[i * 9 + 3] * scale, verts[i * 9 + 4] * scale, verts[i * 9 + 5] * scale),
						btVector3(verts[i * 9 + 6] * scale, verts[i * 9 + 7] * scale, verts[i * 9 + 8] * scale),
						true);
			}
		}
	}
	char lBuf[255];
	sprintf(lBuf, "GroundMesh triangles = %d", groundMesh->getNumTriangles());
	logmsg(lBuf);
	shape = new btBvhTriangleMeshShape(groundMesh, false, true);

	btTransform groundTransform;
	groundTransform.setIdentity();
	//groundTransform.setOrigin(btVector3((WORLD_RIGHT - WORLD_LEFT) / 2,(WORLD_TOP - WORLD_BOTTOM) / 2,0));
	groundTransform.setOrigin(btVector3(0,0,0));

	btScalar mass(0.);
	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(0,0,0);
	if (isDynamic) {
		shape->calculateLocalInertia(mass,localInertia);
	}
	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);
	btBody = new btRigidBody(rbInfo);
	btBody->setUserPointer(this);

	//add the body to the dynamics world
	context->world->btWorld->addRigidBody(btBody);
	*/
}
