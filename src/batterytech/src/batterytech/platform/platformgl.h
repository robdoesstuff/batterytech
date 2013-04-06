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
// Name        : platformgl.h
// Description : OpenGL Support
// Usage       : Include this to use OpenGL ES 1.0, 1.1 and 2.0
//============================================================================

#ifndef PLATFORMGL_H_
#define PLATFORMGL_H_

#ifdef ANDROID_NDK
	#include "android/importgl.h"
    #define OPENGL_ES
#endif /* ANDROID_NDK */

#ifdef _WIN32
#define _WINSOCKAPI_
	#include <windows.h>
#define GLEW_STATIC
	#include "glew.h"
	// needed to map GLES to GL
	#define glOrthof(left,right,bottom,top,near,far) glOrtho(left,right,bottom,top,near,far)
	#define glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar) glFrustum(xmin, xmax, ymin, ymax, zNear, zFar)
    #define OPENGL

#endif /* _WIN32 */

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>
    #define OPENGL_ES
#endif /* IOS */

#if TARGET_OS_MAC && !TARGET_OS_IPHONE
#define GLEW_STATIC
	#import "glew.h"
    #import <OpenGL/gl.h>
    #import <OpenGL/glext.h>
	// needed to map GLES to GL
	#define glOrthof(left,right,bottom,top,near,far) glOrtho(left,right,bottom,top,near,far)
	#define glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar) glFrustum(xmin, xmax, ymin, ymax, zNear, zFar)
    #define OPENGL
#endif /* OSX */

#if defined(linux) and not defined(ANDROID_NDK)
#define GLEW_STATIC
	#include "glew.h"
	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glu.h>
	#include <GL/glx.h>
	// needed to map GLES to GL
	#define glOrthof(left,right,bottom,top,near,far) glOrtho(left,right,bottom,top,near,far)
	#define glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar) glFrustum(xmin, xmax, ymin, ymax, zNear, zFar)
    #define OPENGL
#endif


#if defined(__QNXNTO__)
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#define OPENGL_ES
#endif

#if CHROME
#include <GLES2/gl2.h>
#define GL_API extern
#ifdef GL_APIENTRY
#undef GL_APIENTRY
#endif
#define GL_APIENTRY
#include "chrome/gles1.h"
#define OPENGL_ES
#endif

#include "opengles.h"

#endif /* PLATFORMGENERAL_H_ */
