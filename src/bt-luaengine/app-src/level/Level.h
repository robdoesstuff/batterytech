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
