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

class WorldRenderer: public Renderer {
public:
	WorldRenderer(GraphicsConfiguration *gConfig);
	virtual void unloadLevel();
	virtual void loadLevel();
	virtual void init();
	virtual void render(World *world);
	virtual ~WorldRenderer();
private:
	TextRasterRenderer *textRenderer;
	GraphicsConfiguration *gConfig;
};

#endif /* WORLDRENDERER_H_ */
