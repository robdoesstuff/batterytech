/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
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
