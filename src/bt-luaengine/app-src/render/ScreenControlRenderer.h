/*
 * ScreenControlRenderer.h
 *
 *  Created on: May 28, 2011
 *      Author: rgreen
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
	ShaderProgram *shaderProgram;
	char *fontTag;
};

#endif /* SCREENCONTROLRENDERER_H_ */
