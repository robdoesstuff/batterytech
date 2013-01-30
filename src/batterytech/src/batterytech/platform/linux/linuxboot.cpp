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

// this is because CLOCK_MONOTONIC is in linux/time.h but the correct file to include is sys
// only the indexer cares here.
#ifndef CLOCK_MONOTONIC
	#define CLOCK_MONOTONIC 1
#endif

using namespace std;
using namespace BatteryTech;

// TODO - add icon
// TODO - handle close button
// TODO - swap buffer / run main loop while window moving

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
	swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

	win = XCreateWindow(dpy, root, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, vi->depth, InputOutput,
			vi->visual, CWColormap | CWEventMask, &swa);

	XMapWindow(dpy, win);
	XStoreName(dpy, win, "BatteryTech Demo");

	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);

	GraphicsConfiguration *gConfig = new GraphicsConfiguration();
	gConfig->supportsHWmipmapgen = TRUE;
	gConfig->supportsUVTransform = TRUE;
	gConfig->supportsVBOs = TRUE;
	// TODO - check for GL 2.0+
	gConfig->supportsShaders = TRUE;
	gConfig->supportsFBOs = TRUE;
	BOOL32 initialized = FALSE;
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	BOOL32 isLeftPointerDown = FALSE;
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
					btInit(gConfig, WINDOW_WIDTH, WINDOW_HEIGHT);
					initialized = TRUE;
				}
			} else if (xev.type == KeyPress) {
				btKeyPressed(xev.xkey.keycode, SKEY_NULL);
			} else if (xev.type == MotionNotify) {
				if (isLeftPointerDown) {
					btSetPointerState(0, TRUE, xev.xmotion.x, xev.xmotion.y);
				}
			} else if (xev.type == ButtonPress || xev.type == ButtonRelease) {
				isLeftPointerDown = (xev.type == ButtonPress);
				btSetPointerState(0, isLeftPointerDown, xev.xbutton.x, xev.xbutton.y);
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
