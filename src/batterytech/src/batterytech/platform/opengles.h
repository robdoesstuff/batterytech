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
// Name        : opengles.h
// Description : OpenGL ES Utility Functions
//============================================================================

#ifndef OPENGLES_H_
#define OPENGLES_H_
#include "../primitives.h"
#include <math.h>
#include <stdlib.h>
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix4.h"

#define GLES_VERSION_CONSTANT "OpenGL ES"
#define GLES_PROFILE_COMMON "CM"
#define GLES_PROFILE_COMMON_LITE "CL"
#define GLES_EXT_GENERATE_MIPMAP "generate_mipmap"
#define GLES_EXT_VERTEX_BUFFER_OBJECT "vertex_buffer_object"
#define GLES_EXT_TEXTURE_FLOAT "texture_float"
#define GLES_EXT_DEPTH_TEXTURE "depth_texture"
#define ANDROID_RENDERER_PIXELFLINGER "PixelFlinger"

using namespace BatteryTech;

BOOL32 gles_checkExtension(const char *extension);

void gles_gluPerspective(float fovy, float aspect, float zNear, float zFar);
void gles_gluLookAt(float eyex, float eyey, float eyez,
		float centerx, float centery, float centerz,
		float upx, float upy, float upz);
Vector3f gles_gluProject(const Vector3f &worldCoord, Matrix4f &mvMatrix, Matrix4f &projMatrix, const Vector2f &viewXY, const Vector2f &viewExtents);
Vector3f gles_gluUnProject(const Vector3f &winCoord, Matrix4f &mvMatrix, Matrix4f &projMatrix, const Vector2f &viewXY, const Vector2f &viewExtents);

#endif /* OPENGLES_H_ */
