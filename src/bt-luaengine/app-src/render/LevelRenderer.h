/*
 * BallRenderer.h
 *
 *  Created on: Dec 5, 2010
 *      Author: rgreen
 */

#ifndef LEVELRENDERER_H_
#define LEVELRENDERER_H_

#include <batterytech/render/Renderer.h>
#include "../GameContext.h"
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/util/ManagedArray.h>
#include <batterytech/render/GLModelBinding.h>
#include <batterytech/render/ShaderProgram.h>

using namespace BatteryTech;

class LevelRenderer: public Renderer {
public:
	LevelRenderer(GameContext *context);
	virtual ~LevelRenderer();
	virtual void init(BOOL32 newContext);
	virtual void render(World *world);
private:
	GameContext *context;
	ManagedArray<GLModelBinding> *modelBindings;
	ShaderProgram *shaderProgram;
};

#endif /* LEVELRENDERER_H_ */
