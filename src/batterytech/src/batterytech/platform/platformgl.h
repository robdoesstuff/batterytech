/*
 * platformgeneral.h
 *
 *  Created on: Aug 11, 2010
 *      Author: rgreen
 */

#ifndef PLATFORMGL_H_
#define PLATFORMGL_H_

#ifdef ANDROID_NDK
	#include "platform/android/importgl.h"
#endif /* ANDROID_NDK */

#ifdef _WIN32
	#include <gl/gl.h>
	#include <gl/glu.h>
#endif /* _WIN32 */

#include "opengles.h"

#endif /* PLATFORMGENERAL_H_ */
