/*
 * platformgeneral.h
 *
 *  Created on: Aug 11, 2010
 *      Author: rgreen
 */

#ifndef PLATFORMGL_H_
#define PLATFORMGL_H_

#ifdef ANDROID_NDK
	#include <GLES/gl.h>
	#include <GLES/glext.h>
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#ifdef NO_GLES2_LIB
		// If NO_GLES2_LIB is defined, we have all of the GLES2 functions link to empty functions in boot since they won't be called anyway
		extern void          glAttachShader (GLuint program, GLuint shader);
		extern void          glBindAttribLocation (GLuint program, GLuint index, const char* name);
		extern void          glBindFramebuffer (GLenum target, GLuint framebuffer);
		extern void          glBindRenderbuffer (GLenum target, GLuint renderbuffer);
		extern void          glBlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
		extern void          glBlendEquation ( GLenum mode );
		extern void          glBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha);
		extern void          glBlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
		extern void          glBufferData (GLenum target, GLsizeiptr size, const void* data, GLenum usage);
		extern void          glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
		extern GLenum        glCheckFramebufferStatus (GLenum target);
		extern void          glCompileShader (GLuint shader);
		extern GLuint        glCreateProgram (void);
		extern GLuint        glCreateShader (GLenum type);
		extern void          glDeleteFramebuffers (GLsizei n, const GLuint* framebuffers);
		extern void          glDeleteProgram (GLuint program);
		extern void          glDeleteRenderbuffers (GLsizei n, const GLuint* renderbuffers);
		extern void          glDeleteShader (GLuint shader);
		extern void          glDetachShader (GLuint program, GLuint shader);
		extern void          glDisableVertexAttribArray (GLuint index);
		extern void          glEnableVertexAttribArray (GLuint index);
		extern void          glFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
		extern void          glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
		extern void          glGenFramebuffers (GLsizei n, GLuint* framebuffers);
		extern void          glGenRenderbuffers (GLsizei n, GLuint* renderbuffers);
		extern void          glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
		extern void          glGetActiveUniform (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
		extern void          glGetAttachedShaders (GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
		extern int           glGetAttribLocation (GLuint program, const char* name);
		extern void          glGetBufferParameteriv (GLenum target, GLenum pname, GLint* params);
		extern void          glGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint* params);
		extern void          glGetProgramiv (GLuint program, GLenum pname, GLint* params);
		extern void          glGetProgramInfoLog (GLuint program, GLsizei bufsize, GLsizei* length, char* infolog);
		extern void          glGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint* params);
		extern void          glGetShaderiv (GLuint shader, GLenum pname, GLint* params);
		extern void          glGetShaderInfoLog (GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog);
		extern void          glGetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
		extern void          glGetShaderSource (GLuint shader, GLsizei bufsize, GLsizei* length, char* source);
		extern void          glGetUniformfv (GLuint program, GLint location, GLfloat* params);
		extern void          glGetUniformiv (GLuint program, GLint location, GLint* params);
		extern int           glGetUniformLocation (GLuint program, const char* name);
		extern void          glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat* params);
		extern void          glGetVertexAttribiv (GLuint index, GLenum pname, GLint* params);
		extern void          glGetVertexAttribPointerv (GLuint index, GLenum pname, void** pointer);
		extern GLboolean     glIsFramebuffer (GLuint framebuffer);
		extern GLboolean     glIsProgram (GLuint program);
		extern GLboolean     glIsRenderbuffer (GLuint renderbuffer);
		extern GLboolean     glIsShader (GLuint shader);
		extern void          glLinkProgram (GLuint program);
		extern void          glReleaseShaderCompiler (void);
		extern void          glRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
		extern void          glShaderBinary (GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length);
		extern void          glShaderSource (GLuint shader, GLsizei count, const char** string, const GLint* length);
		extern void          glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask);
		extern void          glStencilMaskSeparate (GLenum face, GLuint mask);
		extern void          glStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
		extern void          glUniform1f (GLint location, GLfloat x);
		extern void          glUniform1fv (GLint location, GLsizei count, const GLfloat* v);
		extern void          glUniform1i (GLint location, GLint x);
		extern void          glUniform1iv (GLint location, GLsizei count, const GLint* v);
		extern void          glUniform2f (GLint location, GLfloat x, GLfloat y);
		extern void          glUniform2fv (GLint location, GLsizei count, const GLfloat* v);
		extern void          glUniform2i (GLint location, GLint x, GLint y);
		extern void          glUniform2iv (GLint location, GLsizei count, const GLint* v);
		extern void          glUniform3f (GLint location, GLfloat x, GLfloat y, GLfloat z);
		extern void          glUniform3fv (GLint location, GLsizei count, const GLfloat* v);
		extern void          glUniform3i (GLint location, GLint x, GLint y, GLint z);
		extern void          glUniform3iv (GLint location, GLsizei count, const GLint* v);
		extern void          glUniform4f (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		extern void          glUniform4fv (GLint location, GLsizei count, const GLfloat* v);
		extern void          glUniform4i (GLint location, GLint x, GLint y, GLint z, GLint w);
		extern void          glUniform4iv (GLint location, GLsizei count, const GLint* v);
		extern void          glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		extern void          glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		extern void          glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		extern void          glUseProgram (GLuint program);
		extern void          glValidateProgram (GLuint program);
		extern void          glVertexAttrib1f (GLuint indx, GLfloat x);
		extern void          glVertexAttrib1fv (GLuint indx, const GLfloat* values);
		extern void          glVertexAttrib2f (GLuint indx, GLfloat x, GLfloat y);
		extern void          glVertexAttrib2fv (GLuint indx, const GLfloat* values);
		extern void          glVertexAttrib3f (GLuint indx, GLfloat x, GLfloat y, GLfloat z);
		extern void          glVertexAttrib3fv (GLuint indx, const GLfloat* values);
		extern void          glVertexAttrib4f (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		extern void          glVertexAttrib4fv (GLuint indx, const GLfloat* values);
		extern void          glVertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr);
	#endif /* NO_GLES2_LIB */
#endif /* ANDROID_NDK */

#ifdef _WIN32
#define _WINSOCKAPI_
	#include <windows.h>
	#include <gl/gl.h>
	#include <gl/glu.h>
	#include "win32/glext.h"
	#include "win32/wglext.h"
	#define glOrthof(left,right,bottom,top,near,far) glOrtho(left,right,bottom,top,near,far)
	#define glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar) glFrustum(xmin, xmax, ymin, ymax, zNear, zFar)

	extern PFNGLCREATESHADERPROC            glCreateShader;
	extern PFNGLATTACHSHADERPROC            glAttachShader;
	extern PFNGLSHADERSOURCEPROC            glShaderSource;
	extern PFNGLGETSHADERIVPROC             glGetShaderiv;
	extern PFNGLCOMPILESHADERPROC           glCompileShader;
	extern PFNGLDETACHSHADERPROC            glDetachShader;
	extern PFNGLDELETESHADERPROC            glDeleteShader;
	extern PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;

	extern PFNGLCREATEPROGRAMPROC           glCreateProgram;
	extern PFNGLLINKPROGRAMPROC             glLinkProgram;
	extern PFNGLGETPROGRAMIVPROC            glGetProgramiv;
	extern PFNGLUSEPROGRAMPROC              glUseProgram;
	extern PFNGLDELETEPROGRAMPROC           glDeleteProgram;
	extern PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;

	extern PFNGLGETUNIFORMLOCATIONPROC     glGetUniformLocation;
	extern PFNGLUNIFORM1IPROC              glUniform1i;
	extern PFNGLUNIFORM1FPROC              glUniform1f;
	extern PFNGLUNIFORM2IPROC              glUniform2i;
	extern PFNGLUNIFORM2FPROC              glUniform2f;
	extern PFNGLUNIFORM3IPROC              glUniform3i;
	extern PFNGLUNIFORM3FPROC              glUniform3f;
	extern PFNGLUNIFORM4IPROC              glUniform4i;
	extern PFNGLUNIFORM4FPROC              glUniform4f;

	extern PFNGLUNIFORMMATRIX4FVPROC		glUniformMatrix4fv;
	extern PFNGLVERTEXATTRIB4FPROC			glVertexAttrib4f;
	extern PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer;
	extern PFNGLENABLEVERTEXATTRIBARRAYPROC	glEnableVertexAttribArray;
	extern PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray;
	extern PFNGLBINDATTRIBLOCATIONPROC		glBindAttribLocation;
	extern PFNGLGETATTRIBLOCATIONPROC		glGetAttribLocation;

#endif /* _WIN32 */

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
#endif /* IOS */

#include "opengles.h"

#endif /* PLATFORMGENERAL_H_ */
