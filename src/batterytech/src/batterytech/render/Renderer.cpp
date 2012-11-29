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
// Name        : Renderer.cpp
// Description : An OpenGL Renderer
// Usage       : Subclass this for object-specific rendering
//============================================================================

#include "Renderer.h"
#include "../decoders/stb_image.h"
#include "../platform/platformgeneral.h"
#include "../Logger.h"
#include <string.h>
#include "../Context.h"
#include "Texture.h"

#define DEBUG_RENDERER TRUE
#define CHECK_GL_ERROR FALSE

namespace BatteryTech {

	Renderer::~Renderer() {
	}

	GLuint Renderer::loadTexture(const char *name, Context *context) {
		GLuint textureId;
		int x, y, n;
		int assetSize = 0;
		unsigned char *fileData = _platform_load_asset(name,
				&assetSize);
		if (!fileData) {
			char buf[1024];
			sprintf(buf, "No asset data found for %s", name);
			logmsg(buf);
			return 0;
		}
		unsigned char *data = stbi_load_from_memory(fileData, assetSize, &x, &y,
				&n, 0);
		if (data) {
			int bytes = x * y * n * sizeof(unsigned char);
			if (DEBUG_RENDERER) {
				char buf[1024];
				sprintf(buf, "Loaded Texture %s (%d enc bytes): %ix%i components=%i bytes=%i", name, assetSize, x, y, n, bytes);
				logmsg(buf);
			}
			GLuint textureIds[1];
			glGenTextures(1, textureIds);
			textureId = textureIds[0];
			Texture::textureSwitches++;
			glBindTexture(GL_TEXTURE_2D, textureId);
			Texture::lastTextureId = textureId;
			if (!context->gConfig->useShaders) {
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			if (context->gConfig->textureFilter == GraphicsConfiguration::NONE) {
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			} else {
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			if (n == 3) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			} else if (n == 4) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			checkGLError("Renderer Load Texture");
			stbi_image_free(data);
		} else {
			char buf[1024];
			sprintf(buf, "Error decoding %s (%d enc bytes)", name, assetSize);
			logmsg(buf);
		}
		_platform_free_asset(fileData);
		return textureId;
	}

	GLuint Renderer::loadShaderFromAsset(GLenum type, const char *assetName) {
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

	GLuint Renderer::loadShader(GLenum type, const char *shaderSrc, const char *assetName) {
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

	void Renderer::logShaderInfo(GLuint obj) {
		GLint infoLen = 0;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 0) {
		  char infoLog[1024];
		  glGetShaderInfoLog(obj, (infoLen > 1024 ? 1024 : infoLen), NULL, infoLog);
		  logmsg(infoLog);
		}
	}

	void Renderer::logProgramInfo(GLuint obj) {
		GLint infoLen = 0;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 0) {
		  char infoLog[1024];
		  glGetProgramInfoLog(obj, (infoLen > 1024 ? 1024 : infoLen), NULL, infoLog);
		  logmsg(infoLog);
		}
	}

	void Renderer::checkGLError(const char *tag) {
#if CHECK_GL_ERROR
		GLenum e = glGetError();
		// loop to clear the error stack
		while (e != GL_NO_ERROR) {
			char buf[255];
			sprintf(buf, "%s - GL Error - %d", tag, e);
			logmsg(buf);
			e = glGetError();
		}
#else
		return;
#endif
	}

	void Renderer::getUniformStructName(char *out, const char *uniformBaseName, int index, const char *elementName) {
		sprintf(out, "%s[%d].%s", uniformBaseName, index, elementName);
	}

}
