/*
 * World.h
 *
 *  Created on: Sep 4, 2010
 *      Author: rgreen
 */

#ifndef WORLD_H_
#define WORLD_H_

#include "../primitives.h"

class World {
public:
	World();
	virtual ~World();
	F32 tickDelta;
	BOOL32 down1;
	BOOL32 down2;
	S32 x1,y1;
	S32 x2,y2;
};

#endif /* WORLD_H_ */
