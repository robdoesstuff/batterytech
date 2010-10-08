/*
 * Renderer.h
 *
 *  Created on: Aug 27, 2010
 *      Author: rgreen
 */

#ifndef RENDERER_H_
#define RENDERER_H_
#include "../game/World.h"
#include "../primitives.h"

class Renderer {
public:
	virtual void unloadLevel() = 0;
	virtual void loadLevel() = 0;
	virtual void init() = 0;
	virtual void render(World *world) = 0;
	virtual ~Renderer();
};

#endif /* RENDERER_H_ */
