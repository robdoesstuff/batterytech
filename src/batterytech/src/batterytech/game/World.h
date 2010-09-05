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
};

#endif /* WORLD_H_ */
