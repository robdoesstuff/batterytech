/*
 * SimpleSpriteRenderer.h
 *
 *  Created on: Dec 2, 2010
 *      Author: rgreen
 */

#ifndef SIMPLESPRITERENDERER_H_
#define SIMPLESPRITERENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/Context.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>

class SimpleSpriteRenderer : Renderer {
public:
	SimpleSpriteRenderer(Context *context, const char *spriteAssetName);
	virtual void init(BOOL32 newContext);
	virtual void render(F32 top, F32 right, F32 bottom, F32 left);
	virtual void render(F32 x, F32 y, F32 width, F32 height, F32 angle);
	virtual ~SimpleSpriteRenderer();
	void setSpriteAssetname(const char* spriteAssetName);
private:
	void drawTexturedQuad(F32 top, F32 right, F32 bottom, F32 left);
	Context *context;
	const char *spriteAssetName;
	GLuint textureId;
	GLuint vertShader, fragShader, program, shaderProjMatrix, shaderMVMatrix, shaderVPosition, shaderUvMap, shaderTex, shaderColorFilter;
};

#endif /* SIMPLESPRITERENDERER_H_ */
