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
// Name        : platformgl.h
// Description : OpenGL Support
// Usage       : Include this to use OpenGL ES 1.0, 1.1 and 2.0
//============================================================================

#ifndef PLATFORMGL_H_
#define PLATFORMGL_H_

#ifdef ANDROID_NDK
	#include "android/importgl.h"
#endif /* ANDROID_NDK */

#ifdef _WIN32
#define _WINSOCKAPI_
	#include <windows.h>
	#include <gl/gl.h>
	#include <gl/glu.h>
	#include "win32/glext.h"
	#include "win32/wglext.h"
	// needed to map GLES to GL
	#define glOrthof(left,right,bottom,top,near,far) glOrtho(left,right,bottom,top,near,far)
	#define glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar) glFrustum(xmin, xmax, ymin, ymax, zNear, zFar)

	// OpenGL 1.5 extensions
	extern PFNGLGENBUFFERSPROC glGenBuffers;
	extern PFNGLBINDBUFFERPROC glBindBuffer;
	extern PFNGLBUFFERDATAPROC glBufferData;
	extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
	extern PFNGLISBUFFERPROC glIsBuffer;
	extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;

	// OpenGL 2.0 extensions
	extern PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
	extern PFNGLDRAWBUFFERSPROC glDrawBuffers;
	extern PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
	extern PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
	extern PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;
	extern PFNGLATTACHSHADERPROC glAttachShader;
	extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
	extern PFNGLCOMPILESHADERPROC glCompileShader;
	extern PFNGLCREATEPROGRAMPROC glCreateProgram;
	extern PFNGLCREATESHADERPROC glCreateShader;
	extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
	extern PFNGLDELETESHADERPROC glDeleteShader;
	extern PFNGLDETACHSHADERPROC glDetachShader;
	extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
	extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	extern PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
	extern PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
	extern PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
	extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
	extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
	extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
	extern PFNGLGETSHADERIVPROC glGetShaderiv;
	extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	extern PFNGLGETSHADERSOURCEPROC glGetShaderSource;
	extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	extern PFNGLGETUNIFORMFVPROC glGetUniformfv;
	extern PFNGLGETUNIFORMIVPROC glGetUniformiv;
	extern PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;
	extern PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;
	extern PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
	extern PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
	extern PFNGLISPROGRAMPROC glIsProgram;
	extern PFNGLISSHADERPROC glIsShader;
	extern PFNGLLINKPROGRAMPROC glLinkProgram;
	extern PFNGLSHADERSOURCEPROC glShaderSource;
	extern PFNGLUSEPROGRAMPROC glUseProgram;
	extern PFNGLUNIFORM1FPROC glUniform1f;
	extern PFNGLUNIFORM2FPROC glUniform2f;
	extern PFNGLUNIFORM3FPROC glUniform3f;
	extern PFNGLUNIFORM4FPROC glUniform4f;
	extern PFNGLUNIFORM1IPROC glUniform1i;
	extern PFNGLUNIFORM2IPROC glUniform2i;
	extern PFNGLUNIFORM3IPROC glUniform3i;
	extern PFNGLUNIFORM4IPROC glUniform4i;
	extern PFNGLUNIFORM1FVPROC glUniform1fv;
	extern PFNGLUNIFORM2FVPROC glUniform2fv;
	extern PFNGLUNIFORM3FVPROC glUniform3fv;
	extern PFNGLUNIFORM4FVPROC glUniform4fv;
	extern PFNGLUNIFORM1IVPROC glUniform1iv;
	extern PFNGLUNIFORM2IVPROC glUniform2iv;
	extern PFNGLUNIFORM3IVPROC glUniform3iv;
	extern PFNGLUNIFORM4IVPROC glUniform4iv;
	extern PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
	extern PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
	extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
	extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;
	extern PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d;
	extern PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv;
	extern PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;
	extern PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;
	extern PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s;
	extern PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv;
	extern PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d;
	extern PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv;
	extern PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f;
	extern PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
	extern PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s;
	extern PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv;
	extern PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d;
	extern PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv;
	extern PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f;
	extern PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
	extern PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s;
	extern PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv;
	extern PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv;
	extern PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv;
	extern PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv;
	extern PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub;
	extern PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv;
	extern PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv;
	extern PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv;
	extern PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv;
	extern PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d;
	extern PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv;
	extern PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;
	extern PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
	extern PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv;
	extern PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s;
	extern PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv;
	extern PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv;
	extern PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv;
	extern PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv;
	extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

#endif /* _WIN32 */

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>
#endif /* IOS */

#if TARGET_OS_MAC && !TARGET_OS_IPHONE
	#import <OpenGL/gl.h>
	#import <OpenGL/glext.h>
	// needed to map GLES to GL
	#define glOrthof(left,right,bottom,top,near,far) glOrtho(left,right,bottom,top,near,far)
	#define glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar) glFrustum(xmin, xmax, ymin, ymax, zNear, zFar)
#endif /* OSX */

#if defined(linux)
	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glu.h>
	#include <GL/glx.h>
	// needed to map GLES to GL
	#define glOrthof(left,right,bottom,top,near,far) glOrtho(left,right,bottom,top,near,far)
	#define glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar) glFrustum(xmin, xmax, ymin, ymax, zNear, zFar)
#endif
#include "opengles.h"

#endif /* PLATFORMGENERAL_H_ */
