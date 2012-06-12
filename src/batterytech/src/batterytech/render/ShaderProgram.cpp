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
// Name        : ShaderProgram.cpp
// Description : Models a Shader Program for easy creation, use and management
//============================================================================

#include "ShaderProgram.h"
#include "../Logger.h"
#include "../platform/platformgeneral.h"
#include <string.h>
#include <stdio.h>
#include "../util/strx.h"

#define DEBUG_SHADERS TRUE

namespace BatteryTech {

int ShaderProgram::binds = 0;
ShaderProgram* ShaderProgram::currentProgram = NULL;

ShaderProgram::ShaderProgram(const char *tag, const char *vertShaderAssetName, const char *fragShaderAssetName) {
	vertShader = fragShader = program = 0;
	this->tag = strdup(tag);
	this->vertShaderAssetName = strdup(vertShaderAssetName);
	this->fragShaderAssetName = strdup(fragShaderAssetName);
	attribLocs = new StrHashTable<GLint>((int)(MAX_VERTEX_ATTRIBUTES * 1.3f));
	// -1 is special and means not found, unlike 0, which seems only to be right for textures, so we use -2 for "NULL" to show that we've already searched and cached
	attribLocs->setNullReturnVal(-2);
	uniformLocs = new StrHashTable<GLint>((int)(MAX_UNIFORMS * 1.3f));
	uniformLocs->setNullReturnVal(-2);
	defines = new ManagedArray<ShaderDefine>(MAX_DEFINES);
}

ShaderProgram::~ShaderProgram() {
	free(tag);
	free(vertShaderAssetName);
	free(fragShaderAssetName);
	attribLocs->deleteElements();
	delete attribLocs;
	uniformLocs->deleteElements();
	delete uniformLocs;
	defines->deleteElements();
	delete defines;
}

void ShaderProgram::invalidateGL() {
	program = 0;
	vertShader = 0;
	fragShader = 0;
	uniformLocs->deleteElements();
	attribLocs->deleteElements();
}

void ShaderProgram::load(BOOL32 force) {
	if (program && !force) {
		// already initialized
		return;
	}
	uniformLocs->deleteElements();
	attribLocs->deleteElements();
	GLint status = 0;
	vertShader = loadShaderFromAsset(GL_VERTEX_SHADER, vertShaderAssetName);
	fragShader = loadShaderFromAsset(GL_FRAGMENT_SHADER, fragShaderAssetName);
	program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(status == GL_FALSE){
		logProgramInfo(program);
	} else {
		// auto find uniforms and attributes
		GLint activeUniforms;
		GLint activeAttributes;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &activeUniforms);
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &activeAttributes);
#if DEBUG_SHADERS
		char buf[255];
		sprintf(buf, "%s uniforms=%d attribs=%d", tag, activeUniforms, activeAttributes);
		logmsg(buf);
#endif
		for (GLuint i = 0; i < (GLuint)activeUniforms; i++) {
			char name[255];
			GLsizei nameLength;
			GLint size;
			GLenum type;
			glGetActiveUniform(program, i, 255, &nameLength, &size, &type, name);
			if (strEndsWith(name, "[0]")) {
				// by default we just want the base name to be consistent
				name[strlen(name)-3] = '\0';
			}
#if DEBUG_SHADERS
			sprintf(buf, "%s uniform %d = %s", tag, i, name);
			logmsg(buf);
#endif
			addUniformLoc(name);
		}
		for (GLuint i = 0; i < (GLuint)activeAttributes; i++) {
			char name[255];
			GLsizei nameLength;
			GLint size;
			GLenum type;
			glGetActiveAttrib(program, i, 255, &nameLength, &size, &type, name);
#if DEBUG_SHADERS
			sprintf(buf, "%s attrib %d = %s", tag, i, name);
			logmsg(buf);
#endif
			addVertexAttributeLoc(name);
		}
	}
}

void ShaderProgram::unload() {
	if (program) {
		glDetachShader(program, vertShader);
		glDetachShader(program, fragShader);
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		glDeleteProgram(program);
	}
	program = 0;
	vertShader = 0;
	fragShader = 0;
}

void ShaderProgram::bind() {
	if( currentProgram == this ) return;
	currentProgram = this;

	binds++;
	glUseProgram(program);
	// enable vertex attrib arrays
	for (StrHashTable<GLint>::Iterator i = attribLocs->getIterator(); i.hasNext;) {
		GLint loc = attribLocs->getNext(i);
		glEnableVertexAttribArray(loc);
	}
}

void ShaderProgram::addDefine(const char *name, const char *value) {
	defines->add(new ShaderDefine(name, value));
}

void ShaderProgram::unbind() {
	// disable vertex attrib arrays
//	for (S32 i = 0; i < attribLocs->getSize(); i++) {
//		glDisableVertexAttribArray(attribLocs->array[i]->loc);
//	}
//	glUseProgram(0);
}

GLint ShaderProgram::addVertexAttributeLoc(const char *name) {
	GLint loc = glGetAttribLocation(program, name);
	if (loc == -1) {
		char buf[1024];
		sprintf(buf, "%s-%s - add - Vertex Attribute %s not found or is invalid", vertShaderAssetName, fragShaderAssetName, name);
		logmsg(buf);
	} else {
		//char buf[255];
		//sprintf(buf, "Added vertex attribute %s at loc %d", name, loc);
		//logmsg(buf);
	}
	attribLocs->put(name, loc);
	return loc;
}

GLint ShaderProgram::addUniformLoc(const char *name) {
	GLint loc = glGetUniformLocation(program, name);
	if (loc == -1) {
		char buf[1024];
		sprintf(buf, "%s-%s - add - Uniform %s not found or is invalid", vertShaderAssetName, fragShaderAssetName, name);
		logmsg(buf);
	} else {
		//char buf[1024];
		//sprintf(buf, "%s-%s - Added uniform %s at loc %d", vertShaderAssetName, fragShaderAssetName, name, loc);
		//logmsg(buf);
	}
	uniformLocs->put(name, loc);
	return loc;
}

GLint ShaderProgram::getVertexAttributeLoc(const char *name) {
	// array-based lookup ok for such small lists.
	GLint loc = attribLocs->get(name);
	if (loc != -2) {
		return loc;
	}
#if DEBUG_SHADERS
	char buf[1024];
	sprintf(buf, "%s-%s - get - Vertex Attribute %s not found, attempting to auto-add", vertShaderAssetName, fragShaderAssetName, name);
	logmsg(buf);
#endif
	return addVertexAttributeLoc(name);
}

GLint ShaderProgram::getUniformLoc(const char *name) {
	GLint loc = uniformLocs->get(name);
	if (loc != -2) {
		return loc;
	}
#if DEBUG_SHADERS
	char buf[1024];
	sprintf(buf, "%s-%s - get - Uniform %s not found, attempting to auto-add", vertShaderAssetName, fragShaderAssetName, name);
	logmsg(buf);
#endif
	return addUniformLoc(name);
}

GLuint ShaderProgram::loadShaderFromAsset(GLenum type, const char *assetName) {
	S32 assetSize;
	unsigned char *fileData = _platform_load_asset(assetName, &assetSize);
	if (fileData) {
		char *shaderText = new char[assetSize+1];
		strncpy(shaderText, (char*)fileData, assetSize);
		shaderText[assetSize] = '\0';
		_platform_free_asset(fileData);
		//logmsg(shaderText);
		GLuint shader = loadShader(type, shaderText, assetName);
		delete [] shaderText;
		return shader;
	} else {
		logmsg("No file data loading shader:");
		logmsg(assetName);
	}
	return 0;
}

GLuint ShaderProgram::loadShader(GLenum type, const char *shaderSrc, const char *assetName) {
	char buf[255];
	sprintf(buf, "Loading Shader %s", assetName);
	logmsg(buf);
	GLuint shader;
	GLint compiled;
	// Create the shader object
	shader = glCreateShader(type);
	if(shader == 0) {
		logmsg("Error - could not create shader of specified type");
		return 0;
	}
	const char *shaderSources[2];
	char defineText[1024];
	defineText[0] = '\0';
	for (S32 i = 0; i < defines->getSize(); i++) {
		strcat(defineText, "#define ");
		strcat(defineText, defines->array[i]->name);
		strcat(defineText, " ");
		strcat(defineText, defines->array[i]->value);
		strcat(defineText, "\n");
	}
	shaderSources[0] = defineText;
	shaderSources[1] = shaderSrc;
	// Load the shader source
	glShaderSource(shader, 2, shaderSources, NULL);
	// Compile the shader
	glCompileShader(shader);
	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(!compiled) {
	   sprintf(buf, "Error loading shader %s", assetName);
	   logmsg(buf);
	   logShaderInfo(shader);
	   glDeleteShader(shader);
	  return 0;
	}
	return shader;
}

void ShaderProgram::logShaderInfo(GLuint obj) {
	GLint infoLen = 0;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLen);
	if(infoLen > 0) {
	  char infoLog[1024];
	  glGetShaderInfoLog(obj, (infoLen > 1024 ? 1024 : infoLen), NULL, infoLog);
	  logmsg(infoLog);
	}
}

void ShaderProgram::logProgramInfo(GLuint obj) {
	GLint infoLen = 0;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLen);
	if(infoLen > 0) {
	  char infoLog[1024];
	  glGetProgramInfoLog(obj, (infoLen > 1024 ? 1024 : infoLen), NULL, infoLog);
	  logmsg(infoLog);
	}
}


}
