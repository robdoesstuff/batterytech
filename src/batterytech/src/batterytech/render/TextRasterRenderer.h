/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : TextRasterRenderer.h
// Description : Renders text from a font-baked texture
//============================================================================

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
	/**
	 * Initializes the TextRasterRenderer for the GL context
	 */
	virtual void init(BOOL32 newContext);
	/**
	 * Renders a line of text using reference coordinate system.
	 * x = position relative to REFERENCE_WIDTH
	 * y = position relative to REFERENCE_HEIGHT
	 * Call startText() before any renders and finishText() after to reset GL states
	 */
	void render(const char *text, F32 x, F32 y);
	/**
	 * Starts rendering text
	 * Must be called before first render()
	 */
	void startText();
	/**
	 * Finishes rendering text
	 * Must be called after render() calls are all complete
	 */
	void finishText();
	/**
	 * Gets the height of the text in reference-size units
	 */
	F32 getHeight();
	/**
	 * Measures the width of a line of text in reference-size units
	 */
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
