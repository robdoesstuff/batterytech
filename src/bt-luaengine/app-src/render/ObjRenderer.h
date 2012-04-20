/*
 * BallRenderer.h
 *
 *  Created on: Dec 5, 2010
 *      Author: rgreen
 */

#ifndef OBJRENDERER_H_
#define OBJRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>

class GameContext;
using namespace BatteryTech;

class RenderItem;

class ObjRenderer: public Renderer {
public:
	ObjRenderer(GameContext *context);
	virtual ~ObjRenderer();
	virtual void init(BOOL32 newContext);
	virtual void render(RenderItem *item);
private:
	GameContext *context;
};

#endif /* BALLRENDERER_H_ */
