/*
 * TextRasterRenderer.h
 *
 *  Created on: Aug 27, 2010
 *      Author: rgreen
 */

#ifndef TEXTRASTERRENDERER_H_
#define TEXTRASTERRENDERER_H_

#include "Renderer.h"

class TextRasterRenderer: public Renderer {
public:
	TextRasterRenderer(const char *assetName, float fontSize);
	virtual void unloadLevel();
	virtual void loadLevel();
	virtual void init();
	virtual void render();
	virtual ~TextRasterRenderer();
};

#endif /* TEXTRASTERRENDERER_H_ */
