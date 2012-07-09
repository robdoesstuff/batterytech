/*
 * BallRenderer.h
 *
 *  Created on: Dec 5, 2010
 *      Author: rgreen
 */

#ifndef BALLRENDERER_H_
#define BALLRENDERER_H_

#include <batterytech/render/Renderer.h>
#include "../GameContext.h"
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>

using namespace BatteryTech;

class BatchSpriteRenderer;

class BallRenderer: public Renderer {
public:
	BallRenderer(GameContext *context);
	virtual ~BallRenderer();
	virtual void init(BOOL32 newGameContext);
	virtual void render(World *world);
private:
	GameContext *context;
};

#endif /* BALLRENDERER_H_ */
