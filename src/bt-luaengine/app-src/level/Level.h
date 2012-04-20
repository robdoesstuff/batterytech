/*
 * Level.h
 *
 *  Created on: Apr 18, 2011
 *      Author: rgreen
 */

#ifndef LEVEL_H_
#define LEVEL_H_

//#include <btBulletCollisionCommon.h>
//#include <btBulletDynamicsCommon.h>
#include "../PhysicsBodyObject.h"
#include <batterytech/importers/obj/ObjScene.h>

class GameContext;

class Level : PhysicsBodyObject {
public:
	Level(GameContext *context);
	virtual ~Level();
	void load(const char *assetName);
	//btRigidBody *btBody;
	//btBvhTriangleMeshShape *shape;
	BatteryTech::ObjScene *scene;
private:
	GameContext *context;
	//btTriangleMesh *groundMesh;
};

#endif /* LEVEL_H_ */
