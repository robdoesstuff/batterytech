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

class Context;

class TextRasterRenderer: public Renderer {
public:
	TextRasterRenderer(Context *context, const char *assetName, F32 fontSize);
	virtual void init(BOOL32 newContext);
	void render(const char *text, F32 x, F32 y);
	void startText();
	void finishText();
	F32 getHeight();
	F32 measureWidth(const char *text);
	virtual ~TextRasterRenderer();
private:
	Context *context;
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
	GLuint ftex;
	const char *aName;
	F32 fontSize;
	S32 bmpWidth, bmpHeight;
	GLuint vertShader, fragShader, program, shaderProjMatrix, shaderMVMatrix, shaderVPosition, shaderUvMap, shaderTex, shaderColorFilter;
};

#endif /* TEXTRASTERRENDERER_H_ */
