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
// Name        : ShaderProgram.h
// Description : Models a Shader Program for easy creation, use and management
//============================================================================

#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#include "../platform/platformgl.h"
#include "../util/ManagedArray.h"

// min is 16
#define MAX_VERTEX_ATTRIBUTES 16
// vert min is 128, frag min is 16
#define MAX_UNIFORMS 144

namespace BatteryTech {

struct ShaderNameLocMap {
	char *name;
	GLint loc;
	ShaderNameLocMap(const char *name, GLint loc) {
		this->name = strdup(name);
		this->loc = loc;
	}
	~ShaderNameLocMap() {
		free(this->name);
	}
};

class ShaderProgram {
public:
	ShaderProgram(const char *vertShaderAssetName, const char *fragShaderAssetName);
	virtual ~ShaderProgram();
	void init(BOOL32 newContext);
	void bind();
	void unbind();
	void addVertexAttributeLoc(const char *name);
	void addUniformLoc(const char *name);
	GLint getVertexAttributeLoc(const char *name);
	GLint getUniformLoc(const char *name);
protected:
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
private:
	GLuint vertShader, fragShader, program;
	char *vertShaderAssetName;
	char *fragShaderAssetName;
	ManagedArray<ShaderNameLocMap> *attribLocs;
	ManagedArray<ShaderNameLocMap> *uniformLocs;
};

}

#endif /* SHADERPROGRAM_H_ */
