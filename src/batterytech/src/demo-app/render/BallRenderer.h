/*
 * BallRenderer.h
 *
 *  Created on: Dec 5, 2010
 *      Author: rgreen
 */

#ifndef BALLRENDERER_H_
#define BALLRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/Context.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include "SimpleSpriteRenderer.h"

class SimpleSpriteRenderer;

class BallRenderer: public Renderer {
public:
	BallRenderer(Context *context);
	virtual ~BallRenderer();
	virtual void init(BOOL32 newContext);
	virtual void render(World *world);
private:
	Context *context;
	SimpleSpriteRenderer *spriteRenderer;
};

#endif /* BALLRENDERER_H_ */
