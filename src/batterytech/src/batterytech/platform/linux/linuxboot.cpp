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

using namespace std;
using namespace BatteryTech;

#define CONSOLE CONSOLE_LOG_ENABLED_WHEN_AVAILABLE
#define DEFAULT_WIDTH WINDOW_WIDTH
#define DEFAULT_HEIGHT WINDOW_HEIGHT

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
	swa.event_mask = ExposureMask | KeyPressMask;

	win = XCreateWindow(dpy, root, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, vi->depth, InputOutput,
			vi->visual, CWColormap | CWEventMask, &swa);

	XMapWindow(dpy, win);
	XStoreName(dpy, win, "BatteryTech Demo");

	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);

	GraphicsConfiguration *gConfig = new GraphicsConfiguration();
	gConfig->supportsHWmipmapgen = true;
	gConfig->supportsUVTransform = true;
	gConfig->supportsVBOs = true;
	BOOL32 initialized = FALSE;
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	while (!quit) {

		if (initialized) {
			long int lastNs = ts.tv_nsec;
			clock_gettime(CLOCK_MONOTONIC, &ts);
			F32 delta = (ts.tv_nsec - lastNs) / 1000000000.f;
			btUpdate(delta);
			btDraw();
			glXSwapBuffers(dpy, win);
		}
		if (XPending(dpy)) {
			XNextEvent(dpy, &xev);
			if (xev.type == Expose) {
				XGetWindowAttributes(dpy, win, &gwa);
				btInit(gConfig, WINDOW_WIDTH, WINDOW_HEIGHT);
				initialized = TRUE;
				//glViewport(0, 0, gwa.width, gwa.height);
			}

			else if (xev.type == KeyPress) {
				glXMakeCurrent(dpy, None, NULL);
				glXDestroyContext(dpy, glc);
				XDestroyWindow(dpy, win);
				XCloseDisplay(dpy);
				exit(0);
			}
		}
	} /* this closes while(1) { */
	return 0;
}

#endif /* linux */
