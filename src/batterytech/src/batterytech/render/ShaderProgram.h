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
// Name        : ShaderProgram.h
// Description : Models a Shader Program for easy creation, use and management
//============================================================================

#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#include "../platform/platformgl.h"
#include "../util/ManagedArray.h"
#include "../util/HashTable.h"

// min is 16
#define MAX_VERTEX_ATTRIBUTES 16
// vert min is 128, frag min is 16
#define MAX_UNIFORMS 144
#define MAX_DEFINES 20

namespace BatteryTech {

struct ShaderDefine {
	char *name;
	char *value;
	ShaderDefine(const char *name, const char *value) {
		this->name = strdup(name);
		if (value) {
			this->value = strdup(value);
		} else {
			this->value = NULL;
		}
	}
	~ShaderDefine() {
		free(this->name);
		free(this->value);
	}
};

class ShaderProgram {
public:
	ShaderProgram(const char *tag, const char *vertShaderAssetName, const char *fragShaderAssetName);
	virtual ~ShaderProgram();
	void invalidateGL();
	void load(BOOL32 force);
	void unload();
	void bind();
	void unbind();
	void addDefine(const char *name, const char *value);
	GLint addVertexAttributeLoc(const char *name);
	GLint addUniformLoc(const char *name);
	GLint getVertexAttributeLoc(const char *name);
	GLint getUniformLoc(const char *name);
    static int binds;
    static ShaderProgram* currentProgram;
	char *tag;
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
	StrHashTable<GLint> *attribLocs;
	StrHashTable<GLint> *uniformLocs;
	ManagedArray<ShaderDefine> *defines;
	BOOL32 vertAttLocsContiguous;
	U32 contigAttribCount;
};

}

#endif /* SHADERPROGRAM_H_ */
