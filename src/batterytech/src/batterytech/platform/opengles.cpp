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
// Name        : opengles.cpp
// Description : OpenGL ES Utility Functions
//============================================================================

#include "opengles.h"
#include "platformgl.h"
#include "platformgeneral.h"
#include <string.h>

using namespace BatteryTech;

BOOL32 gles_checkExtension(const char *extension) {
	return (strstr((const char*) glGetString(GL_EXTENSIONS), extension) != NULL);
}

void gles_gluPerspective(F32 fovy, F32 aspect, F32 zNear, F32 zFar) {
	double xmin, xmax, ymin, ymax;
	ymax = zNear * tan(fovy * 3.14159f / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}

void gles_gluLookAt(float eyex, float eyey, float eyez,
		float centerx, float centery, float centerz,
		float upx, float upy, float upz) {
    GLfloat m[16];
    GLfloat x[3], y[3], z[3];
    GLfloat mag;

    /* Make rotation matrix */

    /* Z vector */
    z[0] = eyex - centerx;
    z[1] = eyey - centery;
    z[2] = eyez - centerz;
    mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
    if (mag) {          /* mpichler, 19950515 */
        z[0] /= mag;
        z[1] /= mag;
        z[2] /= mag;
    }

    /* Y vector */
    y[0] = upx;
    y[1] = upy;
    y[2] = upz;

    /* X vector = Y cross Z */
    x[0] = y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] = y[0] * z[1] - y[1] * z[0];

    /* Recompute Y = Z cross X */
    y[0] = z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] = z[0] * x[1] - z[1] * x[0];

    mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    if (mag) {
        x[0] /= mag;
        x[1] /= mag;
        x[2] /= mag;
    }

    mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
    if (mag) {
        y[0] /= mag;
        y[1] /= mag;
        y[2] /= mag;
    }

#define M(row,col)  m[col*4+row]
    M(0, 0) = x[0];
    M(0, 1) = x[1];
    M(0, 2) = x[2];
    M(0, 3) = 0.0;
    M(1, 0) = y[0];
    M(1, 1) = y[1];
    M(1, 2) = y[2];
    M(1, 3) = 0.0;
    M(2, 0) = z[0];
    M(2, 1) = z[1];
    M(2, 2) = z[2];
    M(2, 3) = 0.0;
    M(3, 0) = 0.0;
    M(3, 1) = 0.0;
    M(3, 2) = 0.0;
    M(3, 3) = 1.0;
#undef M
    glMultMatrixf(m);

    /* Translate Eye to Origin */
    glTranslatef(-eyex, -eyey, -eyez);
}

Vector3f gles_gluProject(const Vector3f &worldCoord, Matrix4f &mvMatrix, Matrix4f &projMatrix, const Vector2f &viewXY, const Vector2f &viewExtents) {
	Matrix4f mat1 = projMatrix * mvMatrix;
	Vector4f v1 = Vector4f(worldCoord.x, worldCoord.y, worldCoord.z, 1.0f);
	v1 = mat1 * v1;
	if (!v1.w) {
		return Vector3f(-1, -1, -1);
	}
	F32 rw = 1.0f / v1.w;
	F32 x = viewXY.x + viewExtents.x * (v1.x * rw + 1.0f) * 0.5f;
	F32 y = viewXY.y + viewExtents.y * (v1.y * rw + 1.0f) * 0.5f;
	F32 z = (v1.z * rw + 1.0f) * 0.5f;
	return Vector3f(x, y, z);
}

Vector3f gles_gluUnProject(const Vector3f &winCoord, Matrix4f &mvMatrix, Matrix4f &projMatrix, const Vector2f &viewXY, const Vector2f &viewExtents) {
	Matrix4f mat1 = projMatrix * mvMatrix;
	Matrix4f invProjMat = mat1.inverse();
	Vector4f srcPoint;
	srcPoint.x = 2.0f * (winCoord.x - viewXY.x) / viewExtents.x - 1.0f;
	srcPoint.y = 2.0f * (winCoord.y - viewXY.y) / viewExtents.y - 1.0f;
	srcPoint.z = 2.0f * winCoord.z - 1.0f;
	srcPoint.w = 1.0f;
	Vector4f out = invProjMat * srcPoint;
	if (out.w == 0.0f) {
		return Vector3f(-1, -1, -1);
	}
	out.w = 1.0f / out.w;
	return Vector3f(out.x*out.w, out.y*out.w, out.z*out.w);
}
