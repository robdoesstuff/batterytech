/*
 * Pinball.h
 *
 *  Created on: Nov 2, 2010
 *      Author: rgreen
 */

#ifndef PINBALL_H_
#define PINBALL_H_

#include "../gameobject/GameObject.h"
#include <Box2D/Dynamics/b2Body.h>
#include "../gameobject/GameObjectMeta.h"

class World;

class Pinball: public GameObject {
public:
	Pinball(Context *context);
	Pinball(Context *context, F32 x, F32 y, F32 angle);
	virtual ~Pinball();
	virtual void init(F32 x, F32 y, F32 angle);
	virtual void update();
	virtual F32 getLinearVelocity();
	virtual void onTouchMove(F32 x, F32 y);
	// called when the touch on this object is lifted (x and y are in world-coordinates)
	virtual GameObjectMeta* getIMeta() { return getMeta(); };
	// returns a singleton instance of this object's meta data
	static GameObjectMeta* getMeta();
	BOOL32 wasInPlay;
private:
	static GameObjectMeta *meta;
};

#endif /* PINBALL_H_ */
