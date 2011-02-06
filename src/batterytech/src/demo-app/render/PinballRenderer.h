/*
 * PinballRenderer.h
 *
 *  Created on: Dec 5, 2010
 *      Author: rgreen
 */

#ifndef PINBALLRENDERER_H_
#define PINBALLRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/Context.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include "SimpleSpriteRenderer.h"

class SimpleSpriteRenderer;

class PinballRenderer: public Renderer {
public:
	PinballRenderer(Context *context);
	virtual ~PinballRenderer();
	virtual void init(BOOL32 newContext);
	virtual void render(World *world);
	virtual void renderBallsRemaining(World *world);
private:
	Context *context;
	SimpleSpriteRenderer *spriteRenderer;
};

#endif /* PINBALLRENDERER_H_ */
