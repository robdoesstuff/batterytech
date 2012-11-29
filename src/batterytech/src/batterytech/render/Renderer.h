/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
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

namespace BatteryTech {

	class Context;

	class Renderer {
	public:
		/**
		 * Initializes the Renderer for the GL context
		 */
		virtual void init(BOOL32 newContext) = 0;
		virtual ~Renderer();
		static void checkGLError(const char *tag);
	protected:
		/**
		 * Loads a texture
		 */
		virtual GLuint loadTexture(const char *name, Context *context);
		/**
		 * Loads a shader from text source
		 */
		GLuint loadShader(GLenum type, const char *shaderSrc, const char *assetName);
		/**
		 * Loads a shader from an asset
		 */
		GLuint loadShaderFromAsset(GLenum type, const char *assetName);
		/**
		 * Logs the shader info from the GPU
		 */
		void logShaderInfo(GLuint obj);
		/**
		 * Logs the program info from the GPU
		 */
		void logProgramInfo(GLuint obj);

		void getUniformStructName(char *out, const char *uniformBaseName, int index, const char *elementName);
	};

}
#endif /* RENDERER_H_ */
