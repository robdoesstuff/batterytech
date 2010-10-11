/*
 * opengles.c
 *
 *  Created on: Oct 10, 2010
 *      Author: rgreen
 */

#include "opengles.h"
#include "platformgl.h"
#include "platformgeneral.h"

BOOL32 gles_checkExtension(const char *extension) {
	return (strstr((const char*) glGetString(GL_EXTENSIONS), extension) != NULL);
}

void gles_gluPerspective(float fovy, float aspect, float zNear, float zFar) {
	double xmin, xmax, ymin, ymax;
	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}


