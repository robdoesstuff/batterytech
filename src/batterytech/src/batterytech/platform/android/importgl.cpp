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
// Name        : importgl.cpp
// Description : Dynamically links OpenGL ES 1 and 2 for Android
//============================================================================

#ifdef ANDROID_NDK

#define IMPORTGL_NO_FNPTR_DEFS
#define IMPORTGL_API
#define IMPORTGL_FNPTRINIT = NULL
#include "importgl.h"
#include <stdlib.h>
#include <dlfcn.h>
static void *sGLESSO = NULL;
static void *sGLES2SO = NULL;

#define IMPORT_FUNC(libName, funcName) do { \
        void *procAddress = (void *)dlsym(libName, #funcName); \
        if (procAddress == NULL) result = 0; \
        *((void **)&FNPTR(funcName)) = procAddress; } while (0)

/* Imports function pointers to selected function calls in OpenGL ES Common
 * or Common Lite profile DLL or shared object. The function pointers are
 * stored as global symbols with equivalent function name but prefixed with
 * "funcPtr_". Standard gl/egl calls are redirected to the function pointers
 * with preprocessor macros (see importgl.h).
 */
int importGLInit(int usingGLES2)
{
    int result = 1;
    sGLES2SO = dlopen("libGLESv2.so", RTLD_NOW);
    if (sGLES2SO && usingGLES2) {
    	// 1.0 functions carried over to 2.0
		IMPORT_FUNC(sGLES2SO, glActiveTexture);
		IMPORT_FUNC(sGLES2SO, glBindBuffer);
		IMPORT_FUNC(sGLES2SO, glBindTexture);
		IMPORT_FUNC(sGLES2SO, glBlendFunc);
		IMPORT_FUNC(sGLES2SO, glBufferData);
		IMPORT_FUNC(sGLES2SO, glBufferSubData);
		IMPORT_FUNC(sGLES2SO, glClear);
		IMPORT_FUNC(sGLES2SO, glClearColor);
		IMPORT_FUNC(sGLES2SO, glClearDepthf);
		IMPORT_FUNC(sGLES2SO, glClearStencil);
		IMPORT_FUNC(sGLES2SO, glClientActiveTexture);
		IMPORT_FUNC(sGLES2SO, glColor4f);
		IMPORT_FUNC(sGLES2SO, glColor4ub);
		IMPORT_FUNC(sGLES2SO, glColor4x);
		IMPORT_FUNC(sGLES2SO, glColorMask);
		IMPORT_FUNC(sGLES2SO, glColorPointer);
		IMPORT_FUNC(sGLES2SO, glCompressedTexImage2D);
		IMPORT_FUNC(sGLES2SO, glCompressedTexSubImage2D);
		IMPORT_FUNC(sGLES2SO, glCopyTexImage2D);
		IMPORT_FUNC(sGLES2SO, glCopyTexSubImage2D);
		IMPORT_FUNC(sGLES2SO, glCullFace);
		IMPORT_FUNC(sGLES2SO, glDeleteBuffers);
		IMPORT_FUNC(sGLES2SO, glDeleteTextures);
		IMPORT_FUNC(sGLES2SO, glDepthFunc);
		IMPORT_FUNC(sGLES2SO, glDepthMask);
		IMPORT_FUNC(sGLES2SO, glDepthRangef);
		IMPORT_FUNC(sGLES2SO, glDepthRangex);
		IMPORT_FUNC(sGLES2SO, glDisable);
		IMPORT_FUNC(sGLES2SO, glDisableClientState);
		IMPORT_FUNC(sGLES2SO, glDrawArrays);
		IMPORT_FUNC(sGLES2SO, glDrawElements);
		IMPORT_FUNC(sGLES2SO, glEnable);
		IMPORT_FUNC(sGLES2SO, glEnableClientState);
		IMPORT_FUNC(sGLES2SO, glFinish);
		IMPORT_FUNC(sGLES2SO, glFlush);
		IMPORT_FUNC(sGLES2SO, glFrontFace);
		IMPORT_FUNC(sGLES2SO, glGenBuffers);
		IMPORT_FUNC(sGLES2SO, glGenTextures);
		IMPORT_FUNC(sGLES2SO, glGetBooleanv);
		IMPORT_FUNC(sGLES2SO, glGetBufferParameteriv);
		IMPORT_FUNC(sGLES2SO, glGetError);
		IMPORT_FUNC(sGLES2SO, glGetFloatv);
		IMPORT_FUNC(sGLES2SO, glGetIntegerv);
		IMPORT_FUNC(sGLES2SO, glGetString);
		IMPORT_FUNC(sGLES2SO, glGetTexParameterfv);
		IMPORT_FUNC(sGLES2SO, glGetTexParameteriv);
		IMPORT_FUNC(sGLES2SO, glHint);
		IMPORT_FUNC(sGLES2SO, glIsBuffer);
		IMPORT_FUNC(sGLES2SO, glIsEnabled);
		IMPORT_FUNC(sGLES2SO, glIsTexture);
		IMPORT_FUNC(sGLES2SO, glLineWidth);
		IMPORT_FUNC(sGLES2SO, glPixelStorei);
		IMPORT_FUNC(sGLES2SO, glPolygonOffset);
		IMPORT_FUNC(sGLES2SO, glPolygonOffsetx);
		IMPORT_FUNC(sGLES2SO, glReadPixels);
		IMPORT_FUNC(sGLES2SO, glSampleCoverage);
		IMPORT_FUNC(sGLES2SO, glScissor);
		IMPORT_FUNC(sGLES2SO, glShadeModel);
		IMPORT_FUNC(sGLES2SO, glStencilFunc);
		IMPORT_FUNC(sGLES2SO, glStencilMask);
		IMPORT_FUNC(sGLES2SO, glStencilOp);
		IMPORT_FUNC(sGLES2SO, glTexImage2D);
		IMPORT_FUNC(sGLES2SO, glTexParameterf);
		IMPORT_FUNC(sGLES2SO, glTexParameterfv);
		IMPORT_FUNC(sGLES2SO, glTexParameteri);
		IMPORT_FUNC(sGLES2SO, glTexParameteriv);
		IMPORT_FUNC(sGLES2SO, glTexSubImage2D);
		IMPORT_FUNC(sGLES2SO, glViewport);
		// 2.0 functions
 		IMPORT_FUNC(sGLES2SO, glAttachShader);
 		IMPORT_FUNC(sGLES2SO, glBindAttribLocation);
 		IMPORT_FUNC(sGLES2SO, glBindFramebuffer);
 		IMPORT_FUNC(sGLES2SO, glBindRenderbuffer);
 		IMPORT_FUNC(sGLES2SO, glBlendColor);
 		IMPORT_FUNC(sGLES2SO, glBlendEquation);
 		IMPORT_FUNC(sGLES2SO, glBlendEquationSeparate);
 		IMPORT_FUNC(sGLES2SO, glBlendFuncSeparate);
 		IMPORT_FUNC(sGLES2SO, glCheckFramebufferStatus);
 		IMPORT_FUNC(sGLES2SO, glCompileShader);
 		IMPORT_FUNC(sGLES2SO, glCreateProgram);
 		IMPORT_FUNC(sGLES2SO, glCreateShader);
 		IMPORT_FUNC(sGLES2SO, glDeleteFramebuffers);
 		IMPORT_FUNC(sGLES2SO, glDeleteProgram);
 		IMPORT_FUNC(sGLES2SO, glDeleteRenderbuffers);
 		IMPORT_FUNC(sGLES2SO, glDeleteShader);
 		IMPORT_FUNC(sGLES2SO, glDetachShader);
 		IMPORT_FUNC(sGLES2SO, glDisableVertexAttribArray);
 		IMPORT_FUNC(sGLES2SO, glEnableVertexAttribArray);
 		IMPORT_FUNC(sGLES2SO, glFramebufferRenderbuffer);
 		IMPORT_FUNC(sGLES2SO, glFramebufferTexture2D);
 		IMPORT_FUNC(sGLES2SO, glGenFramebuffers);
 		IMPORT_FUNC(sGLES2SO, glGenRenderbuffers);
 		IMPORT_FUNC(sGLES2SO, glGetActiveAttrib);
 		IMPORT_FUNC(sGLES2SO, glGetActiveUniform);
 		IMPORT_FUNC(sGLES2SO, glGetAttachedShaders);
 		IMPORT_FUNC(sGLES2SO, glGetAttribLocation);
 		IMPORT_FUNC(sGLES2SO, glGetFramebufferAttachmentParameteriv);
 		IMPORT_FUNC(sGLES2SO, glGetProgramiv);
 		IMPORT_FUNC(sGLES2SO, glGetProgramInfoLog);
 		IMPORT_FUNC(sGLES2SO, glGetRenderbufferParameteriv);
 		IMPORT_FUNC(sGLES2SO, glGetShaderiv);
 		IMPORT_FUNC(sGLES2SO, glGetShaderInfoLog);
 		IMPORT_FUNC(sGLES2SO, glGetShaderPrecisionFormat);
 		IMPORT_FUNC(sGLES2SO, glGetShaderSource);
 		IMPORT_FUNC(sGLES2SO, glGetUniformfv);
 		IMPORT_FUNC(sGLES2SO, glGetUniformiv);
 		IMPORT_FUNC(sGLES2SO, glGetUniformLocation);
 		IMPORT_FUNC(sGLES2SO, glGetVertexAttribfv);
 		IMPORT_FUNC(sGLES2SO, glGetVertexAttribiv);
 		IMPORT_FUNC(sGLES2SO, glGetVertexAttribPointerv);
 		IMPORT_FUNC(sGLES2SO, glIsFramebuffer);
 		IMPORT_FUNC(sGLES2SO, glIsProgram);
 		IMPORT_FUNC(sGLES2SO, glIsRenderbuffer);
 		IMPORT_FUNC(sGLES2SO, glIsShader);
 		IMPORT_FUNC(sGLES2SO, glLinkProgram);
 		IMPORT_FUNC(sGLES2SO, glReleaseShaderCompiler);
 		IMPORT_FUNC(sGLES2SO, glRenderbufferStorage);
 		IMPORT_FUNC(sGLES2SO, glShaderBinary);
 		IMPORT_FUNC(sGLES2SO, glShaderSource);
 		IMPORT_FUNC(sGLES2SO, glStencilFuncSeparate);
 		IMPORT_FUNC(sGLES2SO, glStencilMaskSeparate);
 		IMPORT_FUNC(sGLES2SO, glStencilOpSeparate);
 		IMPORT_FUNC(sGLES2SO, glUniform1f);
 		IMPORT_FUNC(sGLES2SO, glUniform1fv);
 		IMPORT_FUNC(sGLES2SO, glUniform1i);
 		IMPORT_FUNC(sGLES2SO, glUniform1iv);
 		IMPORT_FUNC(sGLES2SO, glUniform2f);
 		IMPORT_FUNC(sGLES2SO, glUniform2fv);
 		IMPORT_FUNC(sGLES2SO, glUniform2i);
 		IMPORT_FUNC(sGLES2SO, glUniform2iv);
 		IMPORT_FUNC(sGLES2SO, glUniform3f);
 		IMPORT_FUNC(sGLES2SO, glUniform3fv);
 		IMPORT_FUNC(sGLES2SO, glUniform3i);
 		IMPORT_FUNC(sGLES2SO, glUniform3iv);
 		IMPORT_FUNC(sGLES2SO, glUniform4f);
 		IMPORT_FUNC(sGLES2SO, glUniform4fv);
 		IMPORT_FUNC(sGLES2SO, glUniform4i);
 		IMPORT_FUNC(sGLES2SO, glUniform4iv);
 		IMPORT_FUNC(sGLES2SO, glUniformMatrix2fv);
 		IMPORT_FUNC(sGLES2SO, glUniformMatrix3fv);
 		IMPORT_FUNC(sGLES2SO, glUniformMatrix4fv);
 		IMPORT_FUNC(sGLES2SO, glUseProgram);
 		IMPORT_FUNC(sGLES2SO, glValidateProgram);
 		IMPORT_FUNC(sGLES2SO, glVertexAttrib1f);
 		IMPORT_FUNC(sGLES2SO, glVertexAttrib1fv);
 		IMPORT_FUNC(sGLES2SO, glVertexAttrib2f);
 		IMPORT_FUNC(sGLES2SO, glVertexAttrib2fv);
 		IMPORT_FUNC(sGLES2SO, glVertexAttrib3f);
 		IMPORT_FUNC(sGLES2SO, glVertexAttrib3fv);
 		IMPORT_FUNC(sGLES2SO, glVertexAttrib4f);
 		IMPORT_FUNC(sGLES2SO, glVertexAttrib4fv);
 		IMPORT_FUNC(sGLES2SO, glVertexAttribPointer);
     }
    sGLESSO = dlopen("libGLESv1_CM.so", RTLD_NOW);
    // don't import anything if 2.0 was successful - it had everything we needed.
    if (!(sGLES2SO && usingGLES2) && sGLESSO) {
		// GLES 1.0/1.1
		IMPORT_FUNC(sGLESSO, glActiveTexture);
		IMPORT_FUNC(sGLESSO, glAlphaFunc);
		IMPORT_FUNC(sGLESSO, glAlphaFuncx);
		IMPORT_FUNC(sGLESSO, glBindBuffer);
		IMPORT_FUNC(sGLESSO, glBindTexture);
		IMPORT_FUNC(sGLESSO, glBlendFunc);
		IMPORT_FUNC(sGLESSO, glBufferData);
		IMPORT_FUNC(sGLESSO, glBufferSubData);
		IMPORT_FUNC(sGLESSO, glClear);
		IMPORT_FUNC(sGLESSO, glClearColor);
		IMPORT_FUNC(sGLESSO, glClearColorx);
		IMPORT_FUNC(sGLESSO, glClearDepthf);
		IMPORT_FUNC(sGLESSO, glClearDepthx);
		IMPORT_FUNC(sGLESSO, glClearStencil);
		IMPORT_FUNC(sGLESSO, glClientActiveTexture);
		IMPORT_FUNC(sGLESSO, glClipPlanef);
		IMPORT_FUNC(sGLESSO, glClipPlanex);
		IMPORT_FUNC(sGLESSO, glColor4f);
		IMPORT_FUNC(sGLESSO, glColor4ub);
		IMPORT_FUNC(sGLESSO, glColor4x);
		IMPORT_FUNC(sGLESSO, glColorMask);
		IMPORT_FUNC(sGLESSO, glColorPointer);
		IMPORT_FUNC(sGLESSO, glCompressedTexImage2D);
		IMPORT_FUNC(sGLESSO, glCompressedTexSubImage2D);
		IMPORT_FUNC(sGLESSO, glCopyTexImage2D);
		IMPORT_FUNC(sGLESSO, glCopyTexSubImage2D);
		IMPORT_FUNC(sGLESSO, glCullFace);
		IMPORT_FUNC(sGLESSO, glDeleteBuffers);
		IMPORT_FUNC(sGLESSO, glDeleteTextures);
		IMPORT_FUNC(sGLESSO, glDepthFunc);
		IMPORT_FUNC(sGLESSO, glDepthMask);
		IMPORT_FUNC(sGLESSO, glDepthRangef);
		IMPORT_FUNC(sGLESSO, glDepthRangex);
		IMPORT_FUNC(sGLESSO, glDisable);
		IMPORT_FUNC(sGLESSO, glDisableClientState);
		IMPORT_FUNC(sGLESSO, glDrawArrays);
		IMPORT_FUNC(sGLESSO, glDrawElements);
		IMPORT_FUNC(sGLESSO, glEnable);
		IMPORT_FUNC(sGLESSO, glEnableClientState);
		IMPORT_FUNC(sGLESSO, glFinish);
		IMPORT_FUNC(sGLESSO, glFlush);
		IMPORT_FUNC(sGLESSO, glFogf);
		IMPORT_FUNC(sGLESSO, glFogfv);
		IMPORT_FUNC(sGLESSO, glFogx);
		IMPORT_FUNC(sGLESSO, glFogxv);
		IMPORT_FUNC(sGLESSO, glFrontFace);
		IMPORT_FUNC(sGLESSO, glFrustumf);
		IMPORT_FUNC(sGLESSO, glFrustumx);
		IMPORT_FUNC(sGLESSO, glGenBuffers);
		IMPORT_FUNC(sGLESSO, glGenTextures);
		IMPORT_FUNC(sGLESSO, glGetBooleanv);
		IMPORT_FUNC(sGLESSO, glGetBufferParameteriv);
		IMPORT_FUNC(sGLESSO, glGetClipPlanef);
		IMPORT_FUNC(sGLESSO, glGetClipPlanex);
		IMPORT_FUNC(sGLESSO, glGetError);
		IMPORT_FUNC(sGLESSO, glGetFixedv);
		IMPORT_FUNC(sGLESSO, glGetFloatv);
		IMPORT_FUNC(sGLESSO, glGetIntegerv);
		IMPORT_FUNC(sGLESSO, glGetLightfv);
		IMPORT_FUNC(sGLESSO, glGetLightxv);
		IMPORT_FUNC(sGLESSO, glGetMaterialfv);
		IMPORT_FUNC(sGLESSO, glGetMaterialxv);
		IMPORT_FUNC(sGLESSO, glGetPointerv);
		IMPORT_FUNC(sGLESSO, glGetString);
		IMPORT_FUNC(sGLESSO, glGetTexEnvfv);
		IMPORT_FUNC(sGLESSO, glGetTexEnviv);
		IMPORT_FUNC(sGLESSO, glGetTexEnvxv);
		IMPORT_FUNC(sGLESSO, glGetTexParameterfv);
		IMPORT_FUNC(sGLESSO, glGetTexParameteriv);
		IMPORT_FUNC(sGLESSO, glGetTexParameterxv);
		IMPORT_FUNC(sGLESSO, glHint);
		IMPORT_FUNC(sGLESSO, glIsBuffer);
		IMPORT_FUNC(sGLESSO, glIsEnabled);
		IMPORT_FUNC(sGLESSO, glIsTexture);
		IMPORT_FUNC(sGLESSO, glLightf);
		IMPORT_FUNC(sGLESSO, glLightfv);
		IMPORT_FUNC(sGLESSO, glLightModelf);
		IMPORT_FUNC(sGLESSO, glLightModelfv);
		IMPORT_FUNC(sGLESSO, glLightModelx);
		IMPORT_FUNC(sGLESSO, glLightModelxv);
		IMPORT_FUNC(sGLESSO, glLightx);
		IMPORT_FUNC(sGLESSO, glLightxv);
		IMPORT_FUNC(sGLESSO, glLineWidth);
		IMPORT_FUNC(sGLESSO, glLineWidthx);
		IMPORT_FUNC(sGLESSO, glLoadIdentity);
		IMPORT_FUNC(sGLESSO, glLoadMatrixf);
		IMPORT_FUNC(sGLESSO, glLoadMatrixx);
		IMPORT_FUNC(sGLESSO, glLogicOp);
		IMPORT_FUNC(sGLESSO, glMaterialf);
		IMPORT_FUNC(sGLESSO, glMaterialfv);
		IMPORT_FUNC(sGLESSO, glMaterialx);
		IMPORT_FUNC(sGLESSO, glMaterialxv);
		IMPORT_FUNC(sGLESSO, glMatrixMode);
		IMPORT_FUNC(sGLESSO, glMultiTexCoord4f);
		IMPORT_FUNC(sGLESSO, glMultiTexCoord4x);
		IMPORT_FUNC(sGLESSO, glMultMatrixf);
		IMPORT_FUNC(sGLESSO, glMultMatrixx);
		IMPORT_FUNC(sGLESSO, glNormal3f);
		IMPORT_FUNC(sGLESSO, glNormal3x);
		IMPORT_FUNC(sGLESSO, glNormalPointer);
		IMPORT_FUNC(sGLESSO, glOrthof);
		IMPORT_FUNC(sGLESSO, glOrthox);
		IMPORT_FUNC(sGLESSO, glPixelStorei);
		IMPORT_FUNC(sGLESSO, glPointParameterf);
		IMPORT_FUNC(sGLESSO, glPointParameterfv);
		IMPORT_FUNC(sGLESSO, glPointParameterx);
		IMPORT_FUNC(sGLESSO, glPointParameterxv);
		IMPORT_FUNC(sGLESSO, glPointSize);
		IMPORT_FUNC(sGLESSO, glPointSizex);
		IMPORT_FUNC(sGLESSO, glPolygonOffset);
		IMPORT_FUNC(sGLESSO, glPolygonOffsetx);
		IMPORT_FUNC(sGLESSO, glPopMatrix);
		IMPORT_FUNC(sGLESSO, glPushMatrix);
		IMPORT_FUNC(sGLESSO, glReadPixels);
		IMPORT_FUNC(sGLESSO, glRotatef);
		IMPORT_FUNC(sGLESSO, glRotatex);
		IMPORT_FUNC(sGLESSO, glSampleCoverage);
		IMPORT_FUNC(sGLESSO, glSampleCoveragex);
		IMPORT_FUNC(sGLESSO, glScalef);
		IMPORT_FUNC(sGLESSO, glScalex);
		IMPORT_FUNC(sGLESSO, glScissor);
		IMPORT_FUNC(sGLESSO, glShadeModel);
		IMPORT_FUNC(sGLESSO, glStencilFunc);
		IMPORT_FUNC(sGLESSO, glStencilMask);
		IMPORT_FUNC(sGLESSO, glStencilOp);
		IMPORT_FUNC(sGLESSO, glTexCoordPointer);
		IMPORT_FUNC(sGLESSO, glTexEnvf);
		IMPORT_FUNC(sGLESSO, glTexEnvfv);
		IMPORT_FUNC(sGLESSO, glTexEnvi);
		IMPORT_FUNC(sGLESSO, glTexEnviv);
		IMPORT_FUNC(sGLESSO, glTexEnvx);
		IMPORT_FUNC(sGLESSO, glTexEnvxv);
		IMPORT_FUNC(sGLESSO, glTexImage2D);
		IMPORT_FUNC(sGLESSO, glTexParameterf);
		IMPORT_FUNC(sGLESSO, glTexParameterfv);
		IMPORT_FUNC(sGLESSO, glTexParameteri);
		IMPORT_FUNC(sGLESSO, glTexParameteriv);
		IMPORT_FUNC(sGLESSO, glTexParameterx);
		IMPORT_FUNC(sGLESSO, glTexParameterxv);
		IMPORT_FUNC(sGLESSO, glTexSubImage2D);
		IMPORT_FUNC(sGLESSO, glTranslatef);
		IMPORT_FUNC(sGLESSO, glTranslatex);
		IMPORT_FUNC(sGLESSO, glVertexPointer);
		IMPORT_FUNC(sGLESSO, glViewport);
    }
     return result;
}


void importGLDeinit() {
	dlclose(sGLESSO);
	dlclose(sGLES2SO);
}


#endif /* ANDROID_NDK */
