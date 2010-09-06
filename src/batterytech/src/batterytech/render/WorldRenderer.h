/*
 * WorldRenderer.h
 *
 *  Created on: Sep 4, 2010
 *      Author: rgreen
 */

#ifndef WORLDRENDERER_H_
#define WORLDRENDERER_H_

#include "Renderer.h"
#include "TextRasterRenderer.h"
#include "GraphicsConfiguration.h"
#include "../primitives.h"

#define REFERENCE_WIDTH 480
#define REFERENCE_HEIGHT 320

class WorldRenderer: public Renderer {
public:
	WorldRenderer(GraphicsConfiguration *gConfig);
	virtual void unloadLevel();
	virtual void loadLevel();
	virtual void init(S32 width, S32 height);
	virtual void render(World *world);
	void setScreenSize(S32 width, S32 height);
	virtual ~WorldRenderer();
private:
	void determineGPUCapabilities();
	TextRasterRenderer *textRenderer;
	GraphicsConfiguration *gConfig;
};

#endif /* WORLDRENDERER_H_ */
