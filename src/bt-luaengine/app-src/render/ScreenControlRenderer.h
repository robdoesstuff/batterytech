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

#ifndef SCREENCONTROLRENDERER_H_
#define SCREENCONTROLRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/render/ShaderProgram.h>

class GameContext;

using namespace BatteryTech;

class ScreenControlRenderer : public Renderer {
public:
	ScreenControlRenderer(GameContext *context, const char *fontTag);
	virtual ~ScreenControlRenderer();
	virtual void init(BOOL32 newContext);
	virtual void render();
private:
	GameContext *context;
	char *fontTag;
};

#endif /* SCREENCONTROLRENDERER_H_ */
