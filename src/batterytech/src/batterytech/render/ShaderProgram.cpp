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
// Name        : ShaderProgram.cpp
// Description : Models a Shader Program for easy creation, use and management
//============================================================================

#include "ShaderProgram.h"
#include "../Logger.h"
#include "../platform/platformgeneral.h"
#include <string.h>
#include <stdio.h>
#include "../util/strx.h"
#include "../batterytech.h"
#include "../Context.h"

namespace BatteryTech {

int ShaderProgram::binds = 0;
ShaderProgram* ShaderProgram::currentProgram = NULL;
static BOOL32 debugShaders = FALSE;

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
	vertAttLocsContiguous = FALSE;
	contigAttribCount = 0;
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
	Property *prop = btGetContext()->appProperties->get("debug_shaders");
	if (prop) {
		debugShaders = prop->getBoolValue();
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
		if (debugShaders) {
			char buf[255];
			sprintf(buf, "%s uniforms=%d attribs=%d", tag, activeUniforms, activeAttributes);
			logmsg(buf);
		}
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
			if (debugShaders) {
				char buf[1024];
				sprintf(buf, "%s uniform %d = %s", tag, i, name);
				logmsg(buf);
			}
			addUniformLoc(name);
		}
		// we say the attribute locations are contiguous if they are like 0,1,2,3 etc but 0 5 8 30 is not nor is 1,2,3,4, must start at 0.
		// this allows for really quick and easy bitwise checks to turn on/off attribute locations when changing shaders.
		BOOL32 isContiguous = TRUE;
		U32 attLocBits = 0;
		S32 maxID = -1;
		S32 minID = 1;
		for (GLuint i = 0; i < (GLuint)activeAttributes; i++) {
			char name[255];
			GLsizei nameLength;
			GLint size;
			GLenum type;
			glGetActiveAttrib(program, i, 255, &nameLength, &size, &type, name);
			if (debugShaders) {
				char buf[1024];
				sprintf(buf, "%s attrib %d = %s", tag, i, name);
				logmsg(buf);
			}
			// determine if sequential starting from 0 and mark as contiguous
			GLint attLoc = addVertexAttributeLoc(name);
			if (attLoc > 32) {
				isContiguous = FALSE;
			}
			if (attLoc > maxID) {
				maxID = attLoc;
			}
			if (attLoc < minID) {
				minID = attLoc;
			}
			attLocBits |= 1 << attLoc;
		}
		if (minID > 0) {
			isContiguous = FALSE;
		} else {
			for (S32 i = 0; i <= maxID; i++) {
				if (!(attLocBits & 1 << i)) {
					isContiguous = FALSE;
					break;
				}
			}
		}
		vertAttLocsContiguous = isContiguous;
		if (isContiguous) {
			contigAttribCount = maxID + 1;
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

/**
 * We have a very optimized bind here.
 *
 * 1) It won't rebind to a shader that's already bound
 * 2) It has optimizations for sequentially numbered vertex attribute locations starting at 0
 * 2a) In this case, which is either 99 or 100% of the time, this will only unbind/bind to deltas in the attribute counts.
 */
void ShaderProgram::bind() {
	if( currentProgram == this ) {
		return;
	}
	ShaderProgram *lastProgram = currentProgram;
	currentProgram = this;
	if (lastProgram) {
		// both programs need contiguous att locs in order to optimize
		if (!lastProgram->vertAttLocsContiguous || !vertAttLocsContiguous) {
			// be safe - unbind all
			for (StrHashTable<GLint>::Iterator i = lastProgram->attribLocs->getIterator(); i.hasNext;) {
					GLint loc = attribLocs->getNext(i);
					glDisableVertexAttribArray(loc);
				}
		} else {
			// only unbind ones we don't need
			for (U32 i = contigAttribCount; i < lastProgram->contigAttribCount; i++) {
				glDisableVertexAttribArray(i);
			}
		}
	}
	binds++;
	glUseProgram(program);
	// enable vertex attrib arrays
	if (lastProgram && lastProgram->vertAttLocsContiguous && vertAttLocsContiguous) {
		// only enable ones beyond what were already enabled
		for (U32 i = lastProgram->contigAttribCount; i < contigAttribCount; i++) {
			glEnableVertexAttribArray(i);
		}
	} else {
		for (StrHashTable<GLint>::Iterator i = attribLocs->getIterator(); i.hasNext;) {
			GLint loc = attribLocs->getNext(i);
			glEnableVertexAttribArray(loc);
		}
	}
}

void ShaderProgram::addDefine(const char *name, const char *value) {
	defines->add(new ShaderDefine(name, value));
}

void ShaderProgram::unbind() {
	// never actually unbind a shader - instead we optimize on the binding of the next.
	// The only caveat is that you must always use this class when dealing with Shaders and Generic Vertex Attributes.
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
	if (debugShaders) {
		char buf[1024];
		sprintf(buf, "%s-%s - get - Vertex Attribute %s not found, attempting to auto-add", vertShaderAssetName, fragShaderAssetName, name);
		logmsg(buf);
	}
	return addVertexAttributeLoc(name);
}

GLint ShaderProgram::getUniformLoc(const char *name) {
	GLint loc = uniformLocs->get(name);
	if (loc != -2) {
		return loc;
	}
	if (debugShaders) {
		char buf[1024];
		sprintf(buf, "%s-%s - get - Uniform %s not found, attempting to auto-add", vertShaderAssetName, fragShaderAssetName, name);
		logmsg(buf);
	}
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
	if (debugShaders) {
		sprintf(buf, "Loading Shader %s", assetName);
		logmsg(buf);
	}
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
