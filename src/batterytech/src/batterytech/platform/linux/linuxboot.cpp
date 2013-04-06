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
// Name        : linuxboot.cpp
// Description : Linux bootstrap
//============================================================================
#if defined(linux)

#include "../../Logger.h"
#include "../platformgl.h"
#include "../../batterytech.h"
#include "../../render/GraphicsConfiguration.h"
#include "../../batterytech_globals.h"
#include "../../primitives.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <sys/time.h>
#include "../../Context.h"
#include "LinuxAudioGW.h"

// this is because CLOCK_MONOTONIC is in linux/time.h but the correct file to include is sys
// only the indexer cares here.
#ifndef CLOCK_MONOTONIC
	#define CLOCK_MONOTONIC 1
#endif

using namespace std;
using namespace BatteryTech;

// TODO - add icon

Display *dpy;
Window root;
GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo *vi;
Colormap cmap;
XSetWindowAttributes swa;
Window win;
GLXContext glc;
XWindowAttributes gwa;
XEvent xev;

BOOL32 leftButtonDown = FALSE;
BOOL32 quit = FALSE;

int main(int argc, char *argv[]) {

	dpy = XOpenDisplay(NULL);

	if (dpy == NULL) {
		printf("\n\tcannot connect to X server\n\n");
		exit(0);
	}

	root = DefaultRootWindow(dpy);

	vi = glXChooseVisual(dpy, 0, att);

	if (vi == NULL) {
		printf("\n\tno appropriate visual found\n\n");
		exit(0);
	} else {
		printf("\n\tvisual %p selected\n", (void *) vi->visualid);
	}/* %p creates hexadecimal output like in glxinfo */

	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

	int height = 800;
	int width = 1280;
	win = XCreateWindow(dpy, root, 0, 0, width, height, 0, vi->depth, InputOutput,
			vi->visual, CWColormap | CWEventMask, &swa);

	XMapWindow(dpy, win);
	XStoreName(dpy, win, "BatteryTech");

	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
	const char *vendor = (const char*)glGetString(GL_VENDOR);
	const char *renderer = (const char*)glGetString(GL_RENDERER);
	const char *version = (const char*)glGetString(GL_VERSION);
	char buf[100];
	sprintf(buf, "OpenGL Vendor [%s]", vendor);
	logmsg(buf);
	sprintf(buf, "OpenGL Renderer [%s]", renderer);
	logmsg(buf);
	sprintf(buf, "OpenGL Version [%s]", version);
	logmsg(buf);
	logmsg("Loading OpenGL extensions");
	glewInit();

	if (!glCreateShader) {
			if (GLEW_ARB_shader_objects && GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GLEW_ARB_shading_language_100) {
				logmsg("Using ARB Shader Objects");
				glCreateShader = glCreateShaderObjectARB;
				glShaderSource = glShaderSourceARB;
				glCompileShader = glCompileShaderARB;
				glGetShaderiv = glGetObjectParameterivARB;
				glGetProgramiv = glGetObjectParameterivARB;
				glGetShaderInfoLog = glGetInfoLogARB;
				glGetProgramInfoLog = glGetInfoLogARB;
				glDeleteProgram = glDeleteObjectARB;
				glDeleteShader = glDeleteObjectARB;
				glUseProgram = glUseProgramObjectARB;
				glAttachShader = glAttachObjectARB;
				glCreateProgram = glCreateProgramObjectARB;
				glValidateProgram = glValidateProgramARB;
				glLinkProgram = glLinkProgramARB;
				glBindAttribLocation = glBindAttribLocationARB;
				glDetachShader = glDetachObjectARB;
				glDisableVertexAttribArray = glDisableVertexAttribArrayARB;
				glEnableVertexAttribArray = glEnableVertexAttribArrayARB;
				glGetActiveAttrib = glGetActiveAttribARB;
				glGetActiveUniform = glGetActiveUniformARB;
				glGetAttachedShaders = glGetAttachedObjectsARB;
				glUniform1f = glUniform1fARB;
				glUniform1fv = glUniform1fvARB;
				glUniform1i = glUniform1iARB;
				glUniform1iv = glUniform1ivARB;
				glUniform2f = glUniform2fARB;
				glUniform2fv = glUniform2fvARB;
				glUniform2i = glUniform2iARB;
				glUniform2iv = glUniform2ivARB;
				glUniform3f = glUniform3fARB;
				glUniform3fv = glUniform3fvARB;
				glUniform3i = glUniform3iARB;
				glUniform3iv = glUniform3ivARB;
				glUniform4f = glUniform4fARB;
				glUniform4fv = glUniform4fvARB;
				glUniform4i = glUniform4iARB;
				glUniform4iv = glUniform4ivARB;
				glUniformMatrix2fv = glUniformMatrix2fvARB;
				glUniformMatrix3fv = glUniformMatrix3fvARB;
				glUniformMatrix4fv = glUniformMatrix4fvARB;
			}
		}
		if (!glGenFramebuffers) {
			if (GLEW_EXT_framebuffer_object) {
				logmsg("Using EXT Framebuffer");
				glGenFramebuffers = glGenFramebuffersEXT;
				glGenRenderbuffers = glGenRenderbuffersEXT;
				glBindRenderbuffer = glBindRenderbufferEXT;
				glRenderbufferStorage = glRenderbufferStorageEXT;
				glFramebufferRenderbuffer = glFramebufferRenderbufferEXT;
				glFramebufferTexture2D = glFramebufferTexture2DEXT;
				glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
				glBindFramebuffer = glBindFramebufferEXT;
				glGetRenderbufferParameteriv = glGetRenderbufferParameterivEXT;
				glIsRenderbuffer = glIsRenderbufferEXT;
				glDeleteRenderbuffers = glDeleteRenderbuffersEXT;
				glIsFramebuffer = glIsFramebufferEXT;
				glDeleteFramebuffers = glDeleteFramebuffersEXT;
				glGetFramebufferAttachmentParameteriv = glGetFramebufferAttachmentParameterivEXT;
				glGetFramebufferParameteriv = glGetFramebufferParameterivEXT;
				glGenerateMipmap = glGenerateMipmapEXT;
			}
		}

	GraphicsConfiguration *gConfig = new GraphicsConfiguration();
	gConfig->supportsHWmipmapgen = TRUE;
	gConfig->supportsUVTransform = TRUE;
	gConfig->supportsVBOs = TRUE;
	if (glCreateShader) {
		logmsg("Shaders supported");
		gConfig->supportsShaders = TRUE;
	} else {
		logmsg("Shaders not supported");
		gConfig->supportsShaders = FALSE;
		gConfig->useShaders = FALSE;
	}
	if (glGenFramebuffers) {
		gConfig->supportsFBOs = TRUE;
	}
	if (GLEW_VERSION_3_0 || GLEW_ARB_texture_float) {
		logmsg("Float textures supported");
		gConfig->supportsFloatTextures = TRUE;
	}
	if (GLEW_VERSION_1_4 || GLEW_ARB_depth_texture) {
		logmsg("Depth textures supported");
		gConfig->supportsDepthTextures = TRUE;
	}
	BOOL32 initialized = FALSE;
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	BOOL32 isLeftPointerDown = FALSE;
	Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, win, &wmDeleteMessage, 1);
	while (!quit) {
		if (initialized) {
			long lastNs = ts.tv_nsec;
			clock_gettime(CLOCK_MONOTONIC, &ts);
			F32 delta = (ts.tv_nsec - lastNs) / 1000000000.f;
			if (delta < 0) {
				delta = 0;
			}
			btUpdate(delta);
			btDraw();
			glXSwapBuffers(dpy, win);
		}
		while (XPending(dpy)) {
			XNextEvent(dpy, &xev);
			if (xev.type == Expose) {
				XGetWindowAttributes(dpy, win, &gwa);
				if (!initialized) {
					btInit(gConfig, width, height);
					initialized = TRUE;
					Context *context = btGetContext();
					if (context->appProperties) {
						const char *windowName = "BatteryTech";
						width = context->appProperties->get("window_width")->getIntValue();
						height = context->appProperties->get("window_height")->getIntValue();
						//console = context->appProperties->get("console_log_enabled_when_available")->getBoolValue();
						btSetScreenSize(width, height);
						windowName = context->appProperties->get("windowed_app_name")->getValue();
						XMoveResizeWindow(dpy, win, 0, 0, width, height);
						XStoreName(dpy, win, windowName);
					}
				}
			} else if (xev.type == KeyPress) {
				btKeyDown(xev.xkey.keycode, SKEY_NULL);
				btKeyPressed(xev.xkey.keycode, SKEY_NULL);
			} else if (xev.type == KeyRelease) {
				btKeyUp(xev.xkey.keycode, SKEY_NULL);
			} else if (xev.type == MotionNotify) {
				if (isLeftPointerDown) {
					btSetPointerState(0, TRUE, xev.xmotion.x, xev.xmotion.y);
				}
			} else if (xev.type == ButtonPress || xev.type == ButtonRelease) {
				isLeftPointerDown = (xev.type == ButtonPress);
				btSetPointerState(0, isLeftPointerDown, xev.xbutton.x, xev.xbutton.y);
			} else if (xev.type == ClientMessage) {
				if (xev.xclient.data.l[0] == wmDeleteMessage) {
					quit = true;
				}
			}
		}
	}
	btRelease();
	glXMakeCurrent(dpy, None, NULL);
	glXDestroyContext(dpy, glc);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
	exit(0);
	return 0;
}

#endif /* linux */
