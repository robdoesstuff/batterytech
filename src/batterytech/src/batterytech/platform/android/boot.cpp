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
// Name        : boot.cpp
// Description : Android bootstrap and JNI linkage
//============================================================================

#ifdef ANDROID_NDK
#include "androidtypes.h"
#include "../platformgeneral.h"
#include "../../batterytech.h"
#include "../../render/GraphicsConfiguration.h"
#include "importgl.h"
#include <jni.h>
#include <android/log.h>
#include "../../util/strx.h"
#include "../opengles.h"
#include "../../audio/AudioManager.h"

#ifdef __cplusplus
extern "C" {
#endif

// the current env for the call
JNIEnv *jnienv;
jobject javaBoot;
extern AudioManager *_andSndMgr; // from androidgeneral
GraphicsConfiguration *gConfig;

void determineGPUCapabilities();

void Java_com_batterypoweredgames_batterytech_Boot_init(JNIEnv* env, jobject thiz, jint width, jint height) {
	jnienv = env;
	javaBoot = thiz;
	gConfig = new GraphicsConfiguration;
	determineGPUCapabilities();
	btInit(gConfig, width, height);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_release(JNIEnv* env, jobject thiz) {
	jnienv = env;
	javaBoot = thiz;
	btRelease();
	delete gConfig;
	jnienv = 0;
	javaBoot = 0;
}
void Java_com_batterypoweredgames_batterytech_Boot_update(JNIEnv* env, jobject thiz, jfloat delta) {
	//__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech Boot", "delta is %f", delta);
	jnienv = env;
	javaBoot = thiz;
	btUpdate((F32)delta);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_draw(JNIEnv* env, jobject thiz) {
	jnienv = env;
	javaBoot = thiz;
	btDraw();
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_suspend(JNIEnv* env, jobject thiz) {
	jnienv = env;
	javaBoot = thiz;
	btSuspend();
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_resume(JNIEnv* env, jobject thiz) {
	jnienv = env;
	javaBoot = thiz;
	btResume();
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_setPointerState(JNIEnv* env, jobject thiz, jint pointerId, jboolean down, jint x, jint y) {
	jnienv = env;
	javaBoot = thiz;
	btSetPointerState(pointerId, down, x, y);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_keyUp(JNIEnv* env, jobject thiz, jint keyCode) {
	jnienv = env;
	javaBoot = thiz;
	btKeyUp(keyCode);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_keyDown(JNIEnv* env, jobject thiz, jint keyCode) {
	jnienv = env;
	javaBoot = thiz;
	btKeyDown(keyCode);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_keyPressed(JNIEnv* env, jobject thiz, jint keyCode) {
	jnienv = env;
	javaBoot = thiz;
	btKeyPressed(keyCode);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_accelerometerChanged(JNIEnv* env, jobject thiz, jfloat x, jfloat y, jfloat z) {
	//__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech Boot", "delta is %f", delta);
	jnienv = env;
	javaBoot = thiz;
	btAccelerometerChanged(x,y,z);
	jnienv = 0;
	javaBoot = 0;
}

void Java_com_batterypoweredgames_batterytech_Boot_fillAudioBuffer(JNIEnv* env, jobject thiz, jshortArray jPCMData, jint length) {
	if (_andSndMgr) {
		signed short *buf = new signed short[length / 2];
		_andSndMgr->fillBuffer(buf, length);
		env->SetShortArrayRegion(jPCMData, 0, length / 2, buf);
		delete [] buf;
	}
}

void determineGPUCapabilities() {
	const char *vendor = (const char*)glGetString(GL_VENDOR);
	const char *renderer = (const char*)glGetString(GL_RENDERER);
	const char *version = (const char*)glGetString(GL_VERSION);
	const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
	// 1.0 = 0, 1.1 = 1, 2.0 = 2
	int ver = 0;
	__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL Vendor %s", vendor);
	__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL Renderer %s", renderer);
	__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL Version %s", version);
	if (strstr(version, "2.0")) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL ES Version 2.0 found");
		ver = 2;
		gConfig->supportsShaders = TRUE;
	} else if (strstr(version, "1.1")) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL ES Version 1.1 found");
		ver = 1;
	}
	if (ver == 0) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "OpenGL ES Version 1.0 found");
	}
	if ((ver > 0) || gles_checkExtension(GLES_EXT_GENERATE_MIPMAP)) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "HW MipMap Gen Supported");
		gConfig->supportsHWmipmapgen = TRUE;
	}
	if ((ver > 0) || gles_checkExtension(GLES_EXT_VERTEX_BUFFER_OBJECT)) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "VBOs Supported");
		gConfig->supportsVBOs = TRUE;
	}
	if (strstr(renderer, "MSM7500")) {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "HW UV Transform not supported");
		gConfig->supportsUVTransform = FALSE;
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", "HW UV Transform supported");
		gConfig->supportsUVTransform = TRUE;
	}
	if (strstr(renderer, ANDROID_RENDERER_PIXELFLINGER)) {
		gConfig->softGPU = TRUE;
	}
}

#ifdef NO_GLES2_LIB
 void          glAttachShader (GLuint program, GLuint shader){}
 void          glBindAttribLocation (GLuint program, GLuint index, const char* name){}
 void          glBindFramebuffer (GLenum target, GLuint framebuffer){}
 void          glBindRenderbuffer (GLenum target, GLuint renderbuffer){}
 void          glBlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha){}
 void          glBlendEquation ( GLenum mode ){}
 void          glBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha){}
 void          glBlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha){}
 void          glBufferData (GLenum target, GLsizeiptr size, const void* data, GLenum usage){}
 void          glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void* data){}
 GLenum        glCheckFramebufferStatus (GLenum target){}
 void          glCompileShader (GLuint shader){}
 GLuint        glCreateProgram (void){}
 GLuint        glCreateShader (GLenum type){}
 void          glDeleteFramebuffers (GLsizei n, const GLuint* framebuffers){}
 void          glDeleteProgram (GLuint program){}
 void          glDeleteRenderbuffers (GLsizei n, const GLuint* renderbuffers){}
 void          glDeleteShader (GLuint shader){}
 void          glDetachShader (GLuint program, GLuint shader){}
 void          glDisableVertexAttribArray (GLuint index){}
 void          glEnableVertexAttribArray (GLuint index){}
 void          glFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer){}
 void          glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level){}
 void          glGenFramebuffers (GLsizei n, GLuint* framebuffers){}
 void          glGenRenderbuffers (GLsizei n, GLuint* renderbuffers){}
 void          glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name){}
 void          glGetActiveUniform (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name){}
 void          glGetAttachedShaders (GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders){}
 int           glGetAttribLocation (GLuint program, const char* name){}
 void          glGetBufferParameteriv (GLenum target, GLenum pname, GLint* params){}
 void          glGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint* params){}
 void          glGetProgramiv (GLuint program, GLenum pname, GLint* params){}
 void          glGetProgramInfoLog (GLuint program, GLsizei bufsize, GLsizei* length, char* infolog){}
 void          glGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint* params){}
 void          glGetShaderiv (GLuint shader, GLenum pname, GLint* params){}
 void          glGetShaderInfoLog (GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog){}
 void          glGetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision){}
 void          glGetShaderSource (GLuint shader, GLsizei bufsize, GLsizei* length, char* source){}
 void          glGetUniformfv (GLuint program, GLint location, GLfloat* params){}
 void          glGetUniformiv (GLuint program, GLint location, GLint* params){}
 int           glGetUniformLocation (GLuint program, const char* name){}
 void          glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat* params){}
 void          glGetVertexAttribiv (GLuint index, GLenum pname, GLint* params){}
 void          glGetVertexAttribPointerv (GLuint index, GLenum pname, void** pointer){}
 GLboolean     glIsFramebuffer (GLuint framebuffer){}
 GLboolean     glIsProgram (GLuint program){}
 GLboolean     glIsRenderbuffer (GLuint renderbuffer){}
 GLboolean     glIsShader (GLuint shader){}
 void          glLinkProgram (GLuint program){}
 void          glReleaseShaderCompiler (void){}
 void          glRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height){}
 void          glShaderBinary (GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length){}
 void          glShaderSource (GLuint shader, GLsizei count, const char** string, const GLint* length){}
 void          glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask){}
 void          glStencilMaskSeparate (GLenum face, GLuint mask){}
 void          glStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass){}
 void          glUniform1f (GLint location, GLfloat x){}
 void          glUniform1fv (GLint location, GLsizei count, const GLfloat* v){}
 void          glUniform1i (GLint location, GLint x){}
 void          glUniform1iv (GLint location, GLsizei count, const GLint* v){}
 void          glUniform2f (GLint location, GLfloat x, GLfloat y){}
 void          glUniform2fv (GLint location, GLsizei count, const GLfloat* v){}
 void          glUniform2i (GLint location, GLint x, GLint y){}
 void          glUniform2iv (GLint location, GLsizei count, const GLint* v){}
 void          glUniform3f (GLint location, GLfloat x, GLfloat y, GLfloat z){}
 void          glUniform3fv (GLint location, GLsizei count, const GLfloat* v){}
 void          glUniform3i (GLint location, GLint x, GLint y, GLint z){}
 void          glUniform3iv (GLint location, GLsizei count, const GLint* v){}
 void          glUniform4f (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w){}
 void          glUniform4fv (GLint location, GLsizei count, const GLfloat* v){}
 void          glUniform4i (GLint location, GLint x, GLint y, GLint z, GLint w){}
 void          glUniform4iv (GLint location, GLsizei count, const GLint* v){}
 void          glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){}
 void          glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){}
 void          glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value){}
 void          glUseProgram (GLuint program){}
 void          glValidateProgram (GLuint program){}
 void          glVertexAttrib1f (GLuint indx, GLfloat x){}
 void          glVertexAttrib1fv (GLuint indx, const GLfloat* values){}
 void          glVertexAttrib2f (GLuint indx, GLfloat x, GLfloat y){}
 void          glVertexAttrib2fv (GLuint indx, const GLfloat* values){}
 void          glVertexAttrib3f (GLuint indx, GLfloat x, GLfloat y, GLfloat z){}
 void          glVertexAttrib3fv (GLuint indx, const GLfloat* values){}
 void          glVertexAttrib4f (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w){}
 void          glVertexAttrib4fv (GLuint indx, const GLfloat* values){}
 void          glVertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr){}
#endif /* NO_GLES2_LIB */

#ifdef __cplusplus
}
#endif


#endif /* _ANDROID */
