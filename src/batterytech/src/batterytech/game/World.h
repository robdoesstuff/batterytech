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

	BOOL32 down1;
	BOOL32 down2;
	S32 x1,y1;
	S32 x2,y2;
	F32 theta;

};

#endif /* WORLD_H_ */
