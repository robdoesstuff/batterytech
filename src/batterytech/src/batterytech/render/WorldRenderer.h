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
#include "../primitives.h"

class WorldRenderer: public Renderer {
public:
	WorldRenderer();
	virtual void unloadLevel();
	virtual void loadLevel();
	virtual void init(S32 width, S32 height);
	virtual void render(World *world);
	void setScreenSize(S32 width, S32 height);
	virtual ~WorldRenderer();
private:
	TextRasterRenderer *textRenderer;
};

#endif /* WORLDRENDERER_H_ */
