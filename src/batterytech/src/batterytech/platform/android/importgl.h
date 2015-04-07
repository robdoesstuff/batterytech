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
// Name        : importgl.h
// Description : Dynamically links OpenGL ES 1 and 2 for Android
//============================================================================

#ifndef IMPORTGL_H_INCLUDED
#define IMPORTGL_H_INCLUDED
#ifdef ANDROID_NDK
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dynamically fetches pointers to the egl & gl functions.
 * Should be called once on application initialization.
 * Returns non-zero on success and 0 on failure.
 */
extern int importGLInit(int usingGLES2);

/* Frees the handle to egl & gl functions library.
 */
extern void importGLDeinit();


#ifndef IMPORTGL_API
#define IMPORTGL_API extern
#endif
#ifndef IMPORTGL_FNPTRINIT
#define IMPORTGL_FNPTRINIT
#endif

#define FNDEF(retType, funcName, args) IMPORTGL_API retType (*funcPtr_##funcName) args IMPORTGL_FNPTRINIT

// GLES 1.0/1.1
FNDEF(void, glAlphaFunc, (GLenum func, GLclampf ref));
FNDEF(void, glClearColor, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha));
FNDEF(void, glClearDepthf, (GLclampf depth));
FNDEF(void, glClipPlanef, (GLenum plane, const GLfloat *equation));
FNDEF(void, glColor4f, (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha));
FNDEF(void, glDepthRangef, (GLclampf zNear, GLclampf zFar));
FNDEF(void, glFogf, (GLenum pname, GLfloat param));
FNDEF(void, glFogfv, (GLenum pname, const GLfloat *params));
FNDEF(void, glFrustumf, (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar));
FNDEF(void, glGetClipPlanef, (GLenum pname, GLfloat eqn[4]));
FNDEF(void, glGetFloatv, (GLenum pname, GLfloat *params));
FNDEF(void, glGetLightfv, (GLenum light, GLenum pname, GLfloat *params));
FNDEF(void, glGetMaterialfv, (GLenum face, GLenum pname, GLfloat *params));
FNDEF(void, glGetTexEnvfv, (GLenum env, GLenum pname, GLfloat *params));
FNDEF(void, glGetTexParameterfv, (GLenum target, GLenum pname, GLfloat *params));
FNDEF(void, glLightModelf, (GLenum pname, GLfloat param));
FNDEF(void, glLightModelfv, (GLenum pname, const GLfloat *params));
FNDEF(void, glLightf, (GLenum light, GLenum pname, GLfloat param));
FNDEF(void, glLightfv, (GLenum light, GLenum pname, const GLfloat *params));
FNDEF(void, glLineWidth, (GLfloat width));
FNDEF(void, glLoadMatrixf, (const GLfloat *m));
FNDEF(void, glMaterialf, (GLenum face, GLenum pname, GLfloat param));
FNDEF(void, glMaterialfv, (GLenum face, GLenum pname, const GLfloat *params));
FNDEF(void, glMultMatrixf, (const GLfloat *m));
FNDEF(void, glMultiTexCoord4f, (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q));
FNDEF(void, glNormal3f, (GLfloat nx, GLfloat ny, GLfloat nz));
FNDEF(void, glOrthof, (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar));
FNDEF(void, glPointParameterf, (GLenum pname, GLfloat param));
FNDEF(void, glPointParameterfv, (GLenum pname, const GLfloat *params));
FNDEF(void, glPointSize, (GLfloat size));
FNDEF(void, glPolygonOffset, (GLfloat factor, GLfloat units));
FNDEF(void, glRotatef, (GLfloat angle, GLfloat x, GLfloat y, GLfloat z));
FNDEF(void, glScalef, (GLfloat x, GLfloat y, GLfloat z));
FNDEF(void, glTexEnvf, (GLenum target, GLenum pname, GLfloat param));
FNDEF(void, glTexEnvfv, (GLenum target, GLenum pname, const GLfloat *params));
FNDEF(void, glTexParameterf, (GLenum target, GLenum pname, GLfloat param));
FNDEF(void, glTexParameterfv, (GLenum target, GLenum pname, const GLfloat *params));
FNDEF(void, glTranslatef, (GLfloat x, GLfloat y, GLfloat z));
FNDEF(void, glActiveTexture, (GLenum texture));
FNDEF(void, glAlphaFuncx, (GLenum func, GLclampx ref));
FNDEF(void, glBindBuffer, (GLenum target, GLuint buffer));
FNDEF(void, glBindTexture, (GLenum target, GLuint texture));
FNDEF(void, glBlendFunc, (GLenum sfactor, GLenum dfactor));
FNDEF(void, glBufferData, (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage));
FNDEF(void, glBufferSubData, (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data));
FNDEF(void, glClear, (GLbitfield mask));
FNDEF(void, glClearColorx, (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha));
FNDEF(void, glClearDepthx, (GLclampx depth));
FNDEF(void, glClearStencil, (GLint s));
FNDEF(void, glClientActiveTexture, (GLenum texture));
FNDEF(void, glClipPlanex, (GLenum plane, const GLfixed *equation));
FNDEF(void, glColor4ub, (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha));
FNDEF(void, glColor4x, (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha));
FNDEF(void, glColorMask, (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha));
FNDEF(void, glColorPointer, (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer));
FNDEF(void, glCompressedTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data));
FNDEF(void, glCompressedTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data));
FNDEF(void, glCopyTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border));
FNDEF(void, glCopyTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height));
FNDEF(void, glCullFace, (GLenum mode));
FNDEF(void, glDeleteBuffers, (GLsizei n, const GLuint *buffers));
FNDEF(void, glDeleteTextures, (GLsizei n, const GLuint *textures));
FNDEF(void, glDepthFunc, (GLenum func));
FNDEF(void, glDepthMask, (GLboolean flag));
FNDEF(void, glDepthRangex, (GLclampx zNear, GLclampx zFar));
FNDEF(void, glDisable, (GLenum cap));
FNDEF(void, glDisableClientState, (GLenum array));
FNDEF(void, glDrawArrays, (GLenum mode, GLint first, GLsizei count));
FNDEF(void, glDrawElements, (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices));
FNDEF(void, glEnable, (GLenum cap));
FNDEF(void, glEnableClientState, (GLenum array));
FNDEF(void, glFinish, (void));
FNDEF(void, glFlush, (void));
FNDEF(void, glFogx, (GLenum pname, GLfixed param));
FNDEF(void, glFogxv, (GLenum pname, const GLfixed *params));
FNDEF(void, glFrontFace, (GLenum mode));
FNDEF(void, glFrustumx, (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar));
FNDEF(void, glGetBooleanv, (GLenum pname, GLboolean *params));
FNDEF(void, glGetBufferParameteriv, (GLenum target, GLenum pname, GLint *params));
FNDEF(void, glGetClipPlanex, (GLenum pname, GLfixed eqn[4]));
FNDEF(void, glGenBuffers, (GLsizei n, GLuint *buffers));
FNDEF(void, glGenTextures, (GLsizei n, GLuint *textures));
FNDEF(GLenum, glGetError, (void));
FNDEF(void, glGetFixedv, (GLenum pname, GLfixed *params));
FNDEF(void, glGetIntegerv, (GLenum pname, GLint *params));
FNDEF(void, glGetLightxv, (GLenum light, GLenum pname, GLfixed *params));
FNDEF(void, glGetMaterialxv, (GLenum face, GLenum pname, GLfixed *params));
FNDEF(void, glGetPointerv, (GLenum pname, void **params));
FNDEF(const GLubyte *, glGetString, (GLenum name));
FNDEF(void, glGetTexEnviv, (GLenum env, GLenum pname, GLint *params));
FNDEF(void, glGetTexEnvxv, (GLenum env, GLenum pname, GLfixed *params));
FNDEF(void, glGetTexParameteriv, (GLenum target, GLenum pname, GLint *params));
FNDEF(void, glGetTexParameterxv, (GLenum target, GLenum pname, GLfixed *params));
FNDEF(void, glHint, (GLenum target, GLenum mode));
FNDEF(GLboolean, glIsBuffer, (GLuint buffer));
FNDEF(GLboolean, glIsEnabled, (GLenum cap));
FNDEF(GLboolean, glIsTexture, (GLuint texture));
FNDEF(void, glLightModelx, (GLenum pname, GLfixed param));
FNDEF(void, glLightModelxv, (GLenum pname, const GLfixed *params));
FNDEF(void, glLightx, (GLenum light, GLenum pname, GLfixed param));
FNDEF(void, glLightxv, (GLenum light, GLenum pname, const GLfixed *params));
FNDEF(void, glLineWidthx, (GLfixed width));
FNDEF(void, glLoadIdentity, (void));
FNDEF(void, glLoadMatrixx, (const GLfixed *m));
FNDEF(void, glLogicOp, (GLenum opcode));
FNDEF(void, glMaterialx, (GLenum face, GLenum pname, GLfixed param));
FNDEF(void, glMaterialxv, (GLenum face, GLenum pname, const GLfixed *params));
FNDEF(void, glMatrixMode, (GLenum mode));
FNDEF(void, glMultMatrixx, (const GLfixed *m));
FNDEF(void, glMultiTexCoord4x, (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q));
FNDEF(void, glNormal3x, (GLfixed nx, GLfixed ny, GLfixed nz));
FNDEF(void, glNormalPointer, (GLenum type, GLsizei stride, const GLvoid *pointer));
FNDEF(void, glOrthox, (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar));
FNDEF(void, glPixelStorei, (GLenum pname, GLint param));
FNDEF(void, glPointParameterx, (GLenum pname, GLfixed param));
FNDEF(void, glPointParameterxv, (GLenum pname, const GLfixed *params));
FNDEF(void, glPointSizex, (GLfixed size));
FNDEF(void, glPolygonOffsetx, (GLfixed factor, GLfixed units));
FNDEF(void, glPopMatrix, (void));
FNDEF(void, glPushMatrix, (void));
FNDEF(void, glReadPixels, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels));
FNDEF(void, glRotatex, (GLfixed angle, GLfixed x, GLfixed y, GLfixed z));
FNDEF(void, glSampleCoverage, (GLclampf value, GLboolean invert));
FNDEF(void, glSampleCoveragex, (GLclampx value, GLboolean invert));
FNDEF(void, glScalex, (GLfixed x, GLfixed y, GLfixed z));
FNDEF(void, glScissor, (GLint x, GLint y, GLsizei width, GLsizei height));
FNDEF(void, glShadeModel, (GLenum mode));
FNDEF(void, glStencilFunc, (GLenum func, GLint ref, GLuint mask));
FNDEF(void, glStencilMask, (GLuint mask));
FNDEF(void, glStencilOp, (GLenum fail, GLenum zfail, GLenum zpass));
FNDEF(void, glTexCoordPointer, (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer));
FNDEF(void, glTexEnvi, (GLenum target, GLenum pname, GLint param));
FNDEF(void, glTexEnvx, (GLenum target, GLenum pname, GLfixed param));
FNDEF(void, glTexEnviv, (GLenum target, GLenum pname, const GLint *params));
FNDEF(void, glTexEnvxv, (GLenum target, GLenum pname, const GLfixed *params));
FNDEF(void, glTexImage2D, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels));
FNDEF(void, glTexParameteri, (GLenum target, GLenum pname, GLint param));
FNDEF(void, glTexParameterx, (GLenum target, GLenum pname, GLfixed param));
FNDEF(void, glTexParameteriv, (GLenum target, GLenum pname, const GLint *params));
FNDEF(void, glTexParameterxv, (GLenum target, GLenum pname, const GLfixed *params));
FNDEF(void, glTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels));
FNDEF(void, glTranslatex, (GLfixed x, GLfixed y, GLfixed z));
FNDEF(void, glVertexPointer, (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer));
FNDEF(void, glViewport, (GLint x, GLint y, GLsizei width, GLsizei height));

// GLES 2.0
FNDEF(void, glAttachShader, (GLuint program, GLuint shader));
FNDEF(void, glBindAttribLocation, (GLuint program, GLuint index, const char* name));
FNDEF(void, glBindFramebuffer, (GLenum target, GLuint framebuffer));
FNDEF(void, glBindRenderbuffer, (GLenum target, GLuint renderbuffer));
FNDEF(void, glBlendColor, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha));
FNDEF(void, glBlendEquation, ( GLenum mode ));
FNDEF(void, glBlendEquationSeparate, (GLenum modeRGB, GLenum modeAlpha));
FNDEF(void, glBlendFuncSeparate, (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha));
FNDEF(GLenum, glCheckFramebufferStatus, (GLenum target));
FNDEF(void, glCompileShader, (GLuint shader));
FNDEF(GLuint, glCreateProgram, (void));
FNDEF(GLuint, glCreateShader, (GLenum type));
FNDEF(void, glDeleteFramebuffers, (GLsizei n, const GLuint* framebuffers));
FNDEF(void, glDeleteProgram, (GLuint program));
FNDEF(void, glDeleteRenderbuffers, (GLsizei n, const GLuint* renderbuffers));
FNDEF(void, glDeleteShader, (GLuint shader));
FNDEF(void, glDetachShader, (GLuint program, GLuint shader));
FNDEF(void, glDisableVertexAttribArray, (GLuint index));
FNDEF(void, glEnableVertexAttribArray, (GLuint index));
FNDEF(void, glFramebufferRenderbuffer, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer));
FNDEF(void, glFramebufferTexture2D, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level));
FNDEF(void, glGenFramebuffers, (GLsizei n, GLuint* framebuffers));
FNDEF(void, glGenRenderbuffers, (GLsizei n, GLuint* renderbuffers));
FNDEF(void, glGetActiveAttrib, (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name));
FNDEF(void, glGetActiveUniform, (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name));
FNDEF(void, glGetAttachedShaders, (GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders));
FNDEF(int, glGetAttribLocation, (GLuint program, const char* name));
FNDEF(void, glGetFramebufferAttachmentParameteriv, (GLenum target, GLenum attachment, GLenum pname, GLint* params));
FNDEF(void, glGetProgramiv, (GLuint program, GLenum pname, GLint* params));
FNDEF(void, glGetProgramInfoLog, (GLuint program, GLsizei bufsize, GLsizei* length, char* infolog));
FNDEF(void, glGetRenderbufferParameteriv, (GLenum target, GLenum pname, GLint* params));
FNDEF(void, glGetShaderiv, (GLuint shader, GLenum pname, GLint* params));
FNDEF(void, glGetShaderInfoLog, (GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog));
FNDEF(void, glGetShaderPrecisionFormat, (GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision));
FNDEF(void, glGetShaderSource, (GLuint shader, GLsizei bufsize, GLsizei* length, char* source));
FNDEF(void, glGetUniformfv, (GLuint program, GLint location, GLfloat* params));
FNDEF(void, glGetUniformiv, (GLuint program, GLint location, GLint* params));
FNDEF(int, glGetUniformLocation, (GLuint program, const char* name));
FNDEF(void, glGetVertexAttribfv, (GLuint index, GLenum pname, GLfloat* params));
FNDEF(void, glGetVertexAttribiv, (GLuint index, GLenum pname, GLint* params));
FNDEF(void, glGetVertexAttribPointerv, (GLuint index, GLenum pname, void** pointer));
FNDEF(GLboolean, glIsFramebuffer, (GLuint framebuffer));
FNDEF(GLboolean, glIsProgram, (GLuint program));
FNDEF(GLboolean, glIsRenderbuffer, (GLuint renderbuffer));
FNDEF(GLboolean, glIsShader, (GLuint shader));
FNDEF(void, glLinkProgram, (GLuint program));
FNDEF(void, glReleaseShaderCompiler, (void));
FNDEF(void, glRenderbufferStorage, (GLenum target, GLenum internalformat, GLsizei width, GLsizei height));
FNDEF(void, glShaderBinary, (GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length));
FNDEF(void, glShaderSource, (GLuint shader, GLsizei count, const char** string, const GLint* length));
FNDEF(void, glStencilFuncSeparate, (GLenum face, GLenum func, GLint ref, GLuint mask));
FNDEF(void, glStencilMaskSeparate, (GLenum face, GLuint mask));
FNDEF(void, glStencilOpSeparate, (GLenum face, GLenum fail, GLenum zfail, GLenum zpass));
FNDEF(void, glUniform1f, (GLint location, GLfloat x));
FNDEF(void, glUniform1fv, (GLint location, GLsizei count, const GLfloat* v));
FNDEF(void, glUniform1i, (GLint location, GLint x));
FNDEF(void, glUniform1iv, (GLint location, GLsizei count, const GLint* v));
FNDEF(void, glUniform2f, (GLint location, GLfloat x, GLfloat y));
FNDEF(void, glUniform2fv, (GLint location, GLsizei count, const GLfloat* v));
FNDEF(void, glUniform2i, (GLint location, GLint x, GLint y));
FNDEF(void, glUniform2iv, (GLint location, GLsizei count, const GLint* v));
FNDEF(void, glUniform3f, (GLint location, GLfloat x, GLfloat y, GLfloat z));
FNDEF(void, glUniform3fv, (GLint location, GLsizei count, const GLfloat* v));
FNDEF(void, glUniform3i, (GLint location, GLint x, GLint y, GLint z));
FNDEF(void, glUniform3iv, (GLint location, GLsizei count, const GLint* v));
FNDEF(void, glUniform4f, (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w));
FNDEF(void, glUniform4fv, (GLint location, GLsizei count, const GLfloat* v));
FNDEF(void, glUniform4i, (GLint location, GLint x, GLint y, GLint z, GLint w));
FNDEF(void, glUniform4iv, (GLint location, GLsizei count, const GLint* v));
FNDEF(void, glUniformMatrix2fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
FNDEF(void, glUniformMatrix3fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
FNDEF(void, glUniformMatrix4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
FNDEF(void, glUseProgram, (GLuint program));
FNDEF(void, glValidateProgram, (GLuint program));
FNDEF(void, glVertexAttrib1f, (GLuint indx, GLfloat x));
FNDEF(void, glVertexAttrib1fv, (GLuint indx, const GLfloat* values));
FNDEF(void, glVertexAttrib2f, (GLuint indx, GLfloat x, GLfloat y));
FNDEF(void, glVertexAttrib2fv, (GLuint indx, const GLfloat* values));
FNDEF(void, glVertexAttrib3f, (GLuint indx, GLfloat x, GLfloat y, GLfloat z));
FNDEF(void, glVertexAttrib3fv, (GLuint indx, const GLfloat* values));
FNDEF(void, glVertexAttrib4f, (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w));
FNDEF(void, glVertexAttrib4fv, (GLuint indx, const GLfloat* values));
FNDEF(void, glVertexAttribPointer, (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr));

#undef FN
#define FNPTR(funcName) funcPtr_##funcName

#ifndef IMPORTGL_NO_FNPTR_DEFS

// GLES 1.0/1.1
#define glActiveTexture    FNPTR(glActiveTexture)
#define glAlphaFunc    FNPTR(glAlphaFunc)
#define glAlphaFuncx    FNPTR(glAlphaFuncx)
#define glBindBuffer    FNPTR(glBindBuffer)
#define glBindTexture    FNPTR(glBindTexture)
#define glBlendFunc    FNPTR(glBlendFunc)
#define glBufferData    FNPTR(glBufferData)
#define glBufferSubData    FNPTR(glBufferSubData)
#define glClear    FNPTR(glClear)
#define glClearColor    FNPTR(glClearColor)
#define glClearColorx    FNPTR(glClearColorx)
#define glClearDepthf    FNPTR(glClearDepthf)
#define glClearDepthx    FNPTR(glClearDepthx)
#define glClearStencil    FNPTR(glClearStencil)
#define glClientActiveTexture    FNPTR(glClientActiveTexture)
#define glClipPlanef    FNPTR(glClipPlanef)
#define glClipPlanex    FNPTR(glClipPlanex)
#define glColor4f    FNPTR(glColor4f)
#define glColor4ub    FNPTR(glColor4ub)
#define glColor4x    FNPTR(glColor4x)
#define glColorMask    FNPTR(glColorMask)
#define glColorPointer    FNPTR(glColorPointer)
#define glCompressedTexImage2D    FNPTR(glCompressedTexImage2D)
#define glCompressedTexSubImage2D    FNPTR(glCompressedTexSubImage2D)
#define glCopyTexImage2D    FNPTR(glCopyTexImage2D)
#define glCopyTexSubImage2D    FNPTR(glCopyTexSubImage2D)
#define glCullFace    FNPTR(glCullFace)
#define glDeleteBuffers    FNPTR(glDeleteBuffers)
#define glDeleteTextures    FNPTR(glDeleteTextures)
#define glDepthFunc    FNPTR(glDepthFunc)
#define glDepthMask    FNPTR(glDepthMask)
#define glDepthRangef    FNPTR(glDepthRangef)
#define glDepthRangex    FNPTR(glDepthRangex)
#define glDisable    FNPTR(glDisable)
#define glDisableClientState    FNPTR(glDisableClientState)
#define glDrawArrays    FNPTR(glDrawArrays)
#define glDrawElements    FNPTR(glDrawElements)
#define glEnable    FNPTR(glEnable)
#define glEnableClientState    FNPTR(glEnableClientState)
#define glFinish    FNPTR(glFinish)
#define glFlush    FNPTR(glFlush)
#define glFogf    FNPTR(glFogf)
#define glFogfv    FNPTR(glFogfv)
#define glFogx    FNPTR(glFogx)
#define glFogxv    FNPTR(glFogxv)
#define glFrontFace    FNPTR(glFrontFace)
#define glFrustumf    FNPTR(glFrustumf)
#define glFrustumx    FNPTR(glFrustumx)
#define glGenBuffers    FNPTR(glGenBuffers)
#define glGenTextures    FNPTR(glGenTextures)
#define glGetBooleanv    FNPTR(glGetBooleanv)
#define glGetBufferParameteriv    FNPTR(glGetBufferParameteriv)
#define glGetClipPlanef    FNPTR(glGetClipPlanef)
#define glGetClipPlanex    FNPTR(glGetClipPlanex)
#define glGetError    FNPTR(glGetError)
#define glGetFixedv    FNPTR(glGetFixedv)
#define glGetFloatv    FNPTR(glGetFloatv)
#define glGetIntegerv    FNPTR(glGetIntegerv)
#define glGetLightfv    FNPTR(glGetLightfv)
#define glGetLightxv    FNPTR(glGetLightxv)
#define glGetMaterialfv    FNPTR(glGetMaterialfv)
#define glGetMaterialxv    FNPTR(glGetMaterialxv)
#define glGetPointerv    FNPTR(glGetPointerv)
#define glGetString    FNPTR(glGetString)
#define glGetTexEnvfv    FNPTR(glGetTexEnvfv)
#define glGetTexEnviv    FNPTR(glGetTexEnviv)
#define glGetTexEnvxv    FNPTR(glGetTexEnvxv)
#define glGetTexParameterfv    FNPTR(glGetTexParameterfv)
#define glGetTexParameteriv    FNPTR(glGetTexParameteriv)
#define glGetTexParameterxv    FNPTR(glGetTexParameterxv)
#define glHint    FNPTR(glHint)
#define glIsBuffer    FNPTR(glIsBuffer)
#define glIsEnabled    FNPTR(glIsEnabled)
#define glIsTexture    FNPTR(glIsTexture)
#define glLightf    FNPTR(glLightf)
#define glLightfv    FNPTR(glLightfv)
#define glLightModelf    FNPTR(glLightModelf)
#define glLightModelfv    FNPTR(glLightModelfv)
#define glLightModelx    FNPTR(glLightModelx)
#define glLightModelxv    FNPTR(glLightModelxv)
#define glLightx    FNPTR(glLightx)
#define glLightxv    FNPTR(glLightxv)
#define glLineWidth    FNPTR(glLineWidth)
#define glLineWidthx    FNPTR(glLineWidthx)
#define glLoadIdentity    FNPTR(glLoadIdentity)
#define glLoadMatrixf    FNPTR(glLoadMatrixf)
#define glLoadMatrixx    FNPTR(glLoadMatrixx)
#define glLogicOp    FNPTR(glLogicOp)
#define glMaterialf    FNPTR(glMaterialf)
#define glMaterialfv    FNPTR(glMaterialfv)
#define glMaterialx    FNPTR(glMaterialx)
#define glMaterialxv    FNPTR(glMaterialxv)
#define glMatrixMode    FNPTR(glMatrixMode)
#define glMultiTexCoord4f    FNPTR(glMultiTexCoord4f)
#define glMultiTexCoord4x    FNPTR(glMultiTexCoord4x)
#define glMultMatrixf    FNPTR(glMultMatrixf)
#define glMultMatrixx    FNPTR(glMultMatrixx)
#define glNormal3f    FNPTR(glNormal3f)
#define glNormal3x    FNPTR(glNormal3x)
#define glNormalPointer    FNPTR(glNormalPointer)
#define glOrthof    FNPTR(glOrthof)
#define glOrthox    FNPTR(glOrthox)
#define glPixelStorei    FNPTR(glPixelStorei)
#define glPointParameterf    FNPTR(glPointParameterf)
#define glPointParameterfv    FNPTR(glPointParameterfv)
#define glPointParameterx    FNPTR(glPointParameterx)
#define glPointParameterxv    FNPTR(glPointParameterxv)
#define glPointSize    FNPTR(glPointSize)
#define glPointSizex    FNPTR(glPointSizex)
#define glPolygonOffset    FNPTR(glPolygonOffset)
#define glPolygonOffsetx    FNPTR(glPolygonOffsetx)
#define glPopMatrix    FNPTR(glPopMatrix)
#define glPushMatrix    FNPTR(glPushMatrix)
#define glReadPixels    FNPTR(glReadPixels)
#define glRotatef    FNPTR(glRotatef)
#define glRotatex    FNPTR(glRotatex)
#define glSampleCoverage    FNPTR(glSampleCoverage)
#define glSampleCoveragex    FNPTR(glSampleCoveragex)
#define glScalef    FNPTR(glScalef)
#define glScalex    FNPTR(glScalex)
#define glScissor    FNPTR(glScissor)
#define glShadeModel    FNPTR(glShadeModel)
#define glStencilFunc    FNPTR(glStencilFunc)
#define glStencilMask    FNPTR(glStencilMask)
#define glStencilOp    FNPTR(glStencilOp)
#define glTexCoordPointer    FNPTR(glTexCoordPointer)
#define glTexEnvf    FNPTR(glTexEnvf)
#define glTexEnvfv    FNPTR(glTexEnvfv)
#define glTexEnvi    FNPTR(glTexEnvi)
#define glTexEnviv    FNPTR(glTexEnviv)
#define glTexEnvx    FNPTR(glTexEnvx)
#define glTexEnvxv    FNPTR(glTexEnvxv)
#define glTexImage2D    FNPTR(glTexImage2D)
#define glTexParameterf    FNPTR(glTexParameterf)
#define glTexParameterfv    FNPTR(glTexParameterfv)
#define glTexParameteri    FNPTR(glTexParameteri)
#define glTexParameteriv    FNPTR(glTexParameteriv)
#define glTexParameterx    FNPTR(glTexParameterx)
#define glTexParameterxv    FNPTR(glTexParameterxv)
#define glTexSubImage2D    FNPTR(glTexSubImage2D)
#define glTranslatef    FNPTR(glTranslatef)
#define glTranslatex    FNPTR(glTranslatex)
#define glVertexPointer    FNPTR(glVertexPointer)
#define glViewport    FNPTR(glViewport)

// GLES 2.0
#define glAttachShader FNPTR(glAttachShader)
#define glBindAttribLocation FNPTR(glBindAttribLocation)
#define glBindFramebuffer FNPTR(glBindFramebuffer)
#define glBindRenderbuffer FNPTR(glBindRenderbuffer)
#define glBlendColor FNPTR(glBlendColor)
#define glBlendEquation FNPTR(glBlendEquation)
#define glBlendEquationSeparate FNPTR(glBlendEquationSeparate)
#define glBlendFuncSeparate FNPTR(glBlendFuncSeparate)
#define glCheckFramebufferStatus FNPTR(glCheckFramebufferStatus)
#define glCompileShader FNPTR(glCompileShader)
#define glCreateProgram FNPTR(glCreateProgram)
#define glCreateShader FNPTR(glCreateShader)
#define glDeleteFramebuffers FNPTR(glDeleteFramebuffers)
#define glDeleteProgram FNPTR(glDeleteProgram)
#define glDeleteRenderbuffers FNPTR(glDeleteRenderbuffers)
#define glDeleteShader FNPTR(glDeleteShader)
#define glDetachShader FNPTR(glDetachShader)
#define glDisableVertexAttribArray FNPTR(glDisableVertexAttribArray)
#define glEnableVertexAttribArray FNPTR(glEnableVertexAttribArray)
#define glFramebufferRenderbuffer FNPTR(glFramebufferRenderbuffer)
#define glFramebufferTexture2D FNPTR(glFramebufferTexture2D)
#define glGenFramebuffers FNPTR(glGenFramebuffers)
#define glGenRenderbuffers FNPTR(glGenRenderbuffers)
#define glGetActiveAttrib FNPTR(glGetActiveAttrib)
#define glGetActiveUniform FNPTR(glGetActiveUniform)
#define glGetAttachedShaders FNPTR(glGetAttachedShaders)
#define glGetAttribLocation FNPTR(glGetAttribLocation)
#define glGetFramebufferAttachmentParameteriv FNPTR(glGetFramebufferAttachmentParameteriv)
#define glGetProgramiv FNPTR(glGetProgramiv)
#define glGetProgramInfoLog FNPTR(glGetProgramInfoLog)
#define glGetRenderbufferParameteriv FNPTR(glGetRenderbufferParameteriv)
#define glGetShaderiv FNPTR(glGetShaderiv)
#define glGetShaderInfoLog FNPTR(glGetShaderInfoLog)
#define glGetShaderPrecisionFormat FNPTR(glGetShaderPrecisionFormat)
#define glGetShaderSource FNPTR(glGetShaderSource)
#define glGetUniformfv FNPTR(glGetUniformfv)
#define glGetUniformiv FNPTR(glGetUniformiv)
#define glGetUniformLocation FNPTR(glGetUniformLocation)
#define glGetVertexAttribfv FNPTR(glGetVertexAttribfv)
#define glGetVertexAttribiv FNPTR(glGetVertexAttribiv)
#define glGetVertexAttribPointerv FNPTR(glGetVertexAttribPointerv)
#define glIsFramebuffer FNPTR(glIsFramebuffer)
#define glIsProgram FNPTR(glIsProgram)
#define glIsRenderbuffer FNPTR(glIsRenderbuffer)
#define glIsShader FNPTR(glIsShader)
#define glLinkProgram FNPTR(glLinkProgram)
#define glReleaseShaderCompiler FNPTR(glReleaseShaderCompiler)
#define glRenderbufferStorage FNPTR(glRenderbufferStorage)
#define glShaderBinary FNPTR(glShaderBinary)
#define glShaderSource FNPTR(glShaderSource)
#define glStencilFuncSeparate FNPTR(glStencilFuncSeparate)
#define glStencilMaskSeparate FNPTR(glStencilMaskSeparate)
#define glStencilOpSeparate FNPTR(glStencilOpSeparate)
#define glUniform1f FNPTR(glUniform1f)
#define glUniform1fv FNPTR(glUniform1fv)
#define glUniform1i FNPTR(glUniform1i)
#define glUniform1iv FNPTR(glUniform1iv)
#define glUniform2f FNPTR(glUniform2f)
#define glUniform2fv FNPTR(glUniform2fv)
#define glUniform2i FNPTR(glUniform2i)
#define glUniform2iv FNPTR(glUniform2iv)
#define glUniform3f FNPTR(glUniform3f)
#define glUniform3fv FNPTR(glUniform3fv)
#define glUniform3i FNPTR(glUniform3i)
#define glUniform3iv FNPTR(glUniform3iv)
#define glUniform4f FNPTR(glUniform4f)
#define glUniform4fv FNPTR(glUniform4fv)
#define glUniform4i FNPTR(glUniform4i)
#define glUniform4iv FNPTR(glUniform4iv)
#define glUniformMatrix2fv FNPTR(glUniformMatrix2fv)
#define glUniformMatrix3fv FNPTR(glUniformMatrix3fv)
#define glUniformMatrix4fv FNPTR(glUniformMatrix4fv)
#define glUseProgram FNPTR(glUseProgram)
#define glValidateProgram FNPTR(glValidateProgram)
#define glVertexAttrib1f FNPTR(glVertexAttrib1f)
#define glVertexAttrib1fv FNPTR(glVertexAttrib1fv)
#define glVertexAttrib2f FNPTR(glVertexAttrib2f)
#define glVertexAttrib2fv FNPTR(glVertexAttrib2fv)
#define glVertexAttrib3f FNPTR(glVertexAttrib3f)
#define glVertexAttrib3fv FNPTR(glVertexAttrib3fv)
#define glVertexAttrib4f FNPTR(glVertexAttrib4f)
#define glVertexAttrib4fv FNPTR(glVertexAttrib4fv)
#define glVertexAttribPointer FNPTR(glVertexAttribPointer)

#endif // !IMPORTGL_NO_FNPTR_DEFS

#ifdef __cplusplus
}
#endif


#endif // ANDROID_NDK
#endif // !IMPORTGL_H_INCLUDED
