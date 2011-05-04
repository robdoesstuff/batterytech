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

namespace BatteryTech {

ShaderProgram::ShaderProgram(const char *vertShaderAssetName, const char *fragShaderAssetName) {
	vertShader = fragShader = program = 0;
	this->vertShaderAssetName = strdup(vertShaderAssetName);
	this->fragShaderAssetName = strdup(fragShaderAssetName);
	attribLocs = new ManagedArray<ShaderNameLocMap>(MAX_VERTEX_ATTRIBUTES);
	uniformLocs = new ManagedArray<ShaderNameLocMap>(MAX_UNIFORMS);
}

ShaderProgram::~ShaderProgram() {
	free(vertShaderAssetName);
	free(fragShaderAssetName);
	attribLocs->deleteElements();
	delete attribLocs;
	uniformLocs->deleteElements();
	delete uniformLocs;
}

void ShaderProgram::init(BOOL32 newContext) {
	if (!newContext && program) {
		glDetachShader(program, vertShader);
		glDetachShader(program, fragShader);
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		glDeleteProgram(program);
	}
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
	}
}

void ShaderProgram::bind() {
	glUseProgram(program);
	// enable vertex attrib arrays
	for (S32 i = 0; i < attribLocs->getSize(); i++) {
		glEnableVertexAttribArray(attribLocs->array[i]->loc);
	}
}

void ShaderProgram::unbind() {
	// disable vertex attrib arrays
	for (S32 i = 0; i < attribLocs->getSize(); i++) {
		glDisableVertexAttribArray(attribLocs->array[i]->loc);
	}
	glUseProgram(0);
}

void ShaderProgram::addVertexAttributeLoc(const char *name) {
	GLint loc = glGetAttribLocation(program, name);
	if (loc == -1) {
		char buf[255];
		sprintf(buf, "Vertex Attribute %s not found or is invalid", name);
		logmsg(buf);
	} else {
		char buf[255];
		sprintf(buf, "Added vertex attribute %s at loc %d", name, loc);
		logmsg(buf);
	}
	attribLocs->add(new ShaderNameLocMap(name, loc));
}

void ShaderProgram::addUniformLoc(const char *name) {
	GLint loc = glGetUniformLocation(program, name);
	if (loc == -1) {
		char buf[255];
		sprintf(buf, "Uniform %s not found or is invalid", name);
		logmsg(buf);
	} else {
		char buf[255];
		sprintf(buf, "Added uniform %s at loc %d", name, loc);
		logmsg(buf);
	}
	uniformLocs->add(new ShaderNameLocMap(name, loc));
}

GLint ShaderProgram::getVertexAttributeLoc(const char *name) {
	// array-based lookup ok for such small lists.
	for (S32 i = 0; i < attribLocs->getSize(); i++) {
		if (strcmp(name, attribLocs->array[i]->name) == 0) {
			return attribLocs->array[i]->loc;
		}
	}
	char buf[255];
	sprintf(buf, "Vertex Attribute %s not found", name);
	logmsg(buf);
	return 0;
}

GLint ShaderProgram::getUniformLoc(const char *name) {
	for (S32 i = 0; i < uniformLocs->getSize(); i++) {
		if (strcmp(name, uniformLocs->array[i]->name) == 0) {
			return uniformLocs->array[i]->loc;
		}
	}
	char buf[255];
	sprintf(buf, "Uniform %s not found", name);
	logmsg(buf);
	return 0;
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
   GLuint shader;
   GLint compiled;
   // Create the shader object
   shader = glCreateShader(type);
   if(shader == 0) {
	   return 0;
   }
   // Load the shader source
   glShaderSource(shader, 1, &shaderSrc, NULL);
   // Compile the shader
   glCompileShader(shader);
   // Check the compile status
   glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
   if(!compiled) {
	   char buf[255];
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
