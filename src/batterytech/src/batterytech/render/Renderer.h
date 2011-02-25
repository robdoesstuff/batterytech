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
// Name        : Renderer.h
// Description : An OpenGL Renderer
// Usage       : Subclass this for object-specific rendering
//============================================================================

#ifndef RENDERER_H_
#define RENDERER_H_

#include "../platform/platformgl.h"
#include "../primitives.h"
#include "GraphicsConfiguration.h"

class Context;

class Renderer {
public:
	virtual void init(BOOL32 newContext) = 0;
	virtual ~Renderer();
protected:
	virtual GLuint loadTexture(const char *name, Context *context);
	GLuint loadShader(GLenum type, const char *shaderSrc);
	GLuint loadShaderFromAsset(GLenum type, const char *assetName);
	void logShaderInfo(GLuint obj);
	void logProgramInfo(GLuint obj);
};

#endif /* RENDERER_H_ */
