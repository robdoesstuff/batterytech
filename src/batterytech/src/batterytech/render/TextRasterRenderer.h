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
#include "GraphicsConfiguration.h"
#include "../../demo-app/World.h"

class TextRasterRenderer: public Renderer {
public:
	TextRasterRenderer(GraphicsConfiguration *gConfig, const char *assetName, float fontSize);
	virtual void unloadLevel();
	virtual void loadLevel();
	virtual void init(BOOL32 newContext);
	virtual void render(World *world);
	void render(const char *text, F32 x, F32 y);
	void startText();
	void finishText();
	F32 getHeight();
	F32 measureWidth(const char *text);
	virtual ~TextRasterRenderer();
private:
	GraphicsConfiguration *gConfig;
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
	GLuint ftex;
	const char *aName;
};

#endif /* TEXTRASTERRENDERER_H_ */
