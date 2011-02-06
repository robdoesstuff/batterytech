/*
 * Renderer.cpp
 *
 *  Created on: Aug 27, 2010
 *      Author: rgreen
 */

#include "Renderer.h"
#include "../decoders/stb_image.h"
#include "../platform/platformgeneral.h"
#include "../Logger.h"

Renderer::~Renderer() {
}

GLuint Renderer::loadTexture(const char *name) {
	GLuint textureId;
	logmsg("Loading texture:");
	logmsg(name);
	int x, y, n;
	int assetSize = 0;
	unsigned char *fileData = _platform_load_asset(name,
			&assetSize);
	if (fileData) {
		char buf[75];
		sprintf(buf, "Loaded %i bytes of raw image data", assetSize);
		logmsg(buf);
	}
	unsigned char *data = stbi_load_from_memory(fileData, assetSize, &x, &y,
			&n, 0);
	if (data) {
		int bytes = x * y * n * sizeof(unsigned char);
		char buf[100];
		sprintf(buf, "Bitmap is %ix%i color components=%i bytes=%i", x, y, n,
				bytes);
		logmsg(buf);
		GLuint textureIds[1];
		glGenTextures(1, textureIds);
		textureId = textureIds[0];
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		if (n == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		} else if (n == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		GLenum error = glGetError();
		if (error) {
			logmsg("GL Error");
		}
		stbi_image_free(data);
	}
	_platform_free_asset(fileData);
	return textureId;
}
