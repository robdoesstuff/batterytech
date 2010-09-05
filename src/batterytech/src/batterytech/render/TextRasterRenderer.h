/*
 * TextRasterRenderer.h
 *
 *  Created on: Aug 27, 2010
 *      Author: rgreen
 */

#ifndef TEXTRASTERRENDERER_H_
#define TEXTRASTERRENDERER_H_

#include "Renderer.h"
#include "../decoders/stb_truetype.h"
#include "../platform/platformgl.h"

class TextRasterRenderer: public Renderer {
public:
	TextRasterRenderer(const char *assetName, float fontSize);
	virtual void unloadLevel();
	virtual void loadLevel();
	virtual void init(S32 width, S32 height);
	virtual void render(World *world);
	virtual ~TextRasterRenderer();
private:
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
	GLuint ftex;
	const char *aName;
};

#endif /* TEXTRASTERRENDERER_H_ */
