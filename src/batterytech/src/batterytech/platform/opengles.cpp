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
// Name        : opengles.cpp
// Description : OpenGL ES Utility Functions
//============================================================================

#include "opengles.h"
#include "platformgl.h"
#include "platformgeneral.h"
#include <string.h>

BOOL32 gles_checkExtension(const char *extension) {
	return (strstr((const char*) glGetString(GL_EXTENSIONS), extension) != NULL);
}

void gles_gluPerspective(float fovy, float aspect, float zNear, float zFar) {
	double xmin, xmax, ymin, ymax;
	ymax = zNear * tan(fovy * 3.14159f / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}


