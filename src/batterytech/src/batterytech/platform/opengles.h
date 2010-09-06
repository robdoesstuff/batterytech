/*
 * opengles.h
 *
 *  Created on: Sep 6, 2010
 *      Author: rgreen
 */

#ifndef OPENGLES_H_
#define OPENGLES_H_
#include "platformgl.h"
#include "../primitives.h"

#define GLES_VERSION_CONSTANT "OpenGL ES-"
#define GLES_PROFILE_COMMON "CM"
#define GLES_PROFILE_COMMON_LITE "CL"
#define GLES_EXT_GENERATE_MIPMAP "generate_mipmap"
#define GLES_EXT_VERTEX_BUFFER_OBJECT "vertex_buffer_object"
#define ANDROID_RENDERER_PIXELFLINGER "PixelFlinger"

BOOL32 gles_checkExtension(const char *extension) {
	return (strstr((const char*)glGetString(GL_EXTENSIONS), extension) != NULL);
}

#endif /* OPENGLES_H_ */
