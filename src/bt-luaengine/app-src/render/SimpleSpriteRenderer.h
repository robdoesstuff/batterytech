/*
 * SimpleSpriteRenderer.h
 *
 *  Created on: Dec 2, 2010
 *      Author: rgreen
 */

#ifndef SIMPLESPRITERENDERER_H_
#define SIMPLESPRITERENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/render/ShaderProgram.h>
#include "RenderItem.h"

using namespace BatteryTech;

class GameContext;

class SimpleSpriteRenderer : Renderer {
public:
	SimpleSpriteRenderer(GameContext *context);
	virtual void init(BOOL32 newContext);
	virtual void render(F32 top, F32 right, F32 bottom, F32 left);
	virtual void render(F32 x, F32 y, F32 width, F32 height, F32 angle);
	virtual void render(RenderItem *item);
	virtual ~SimpleSpriteRenderer();
private:
	GameContext *context;
	ShaderProgram *shaderProgram;
};

#endif /* SIMPLESPRITERENDERER_H_ */
