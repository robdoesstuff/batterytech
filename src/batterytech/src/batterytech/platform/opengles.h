/*
 * opengles.h
 *
 *  Created on: Sep 6, 2010
 *      Author: rgreen
 */

#ifndef OPENGLES_H_
#define OPENGLES_H_
#include "../primitives.h"
#include <Math.h>
#include <stdlib.h>

#define GLES_VERSION_CONSTANT "OpenGL ES-"
#define GLES_PROFILE_COMMON "CM"
#define GLES_PROFILE_COMMON_LITE "CL"
#define GLES_EXT_GENERATE_MIPMAP "generate_mipmap"
#define GLES_EXT_VERTEX_BUFFER_OBJECT "vertex_buffer_object"
#define ANDROID_RENDERER_PIXELFLINGER "PixelFlinger"

BOOL32 gles_checkExtension(const char *extension);

void gles_gluPerspective(float fovy, float aspect, float zNear, float zFar);

#endif /* OPENGLES_H_ */
