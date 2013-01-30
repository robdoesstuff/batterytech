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
// Name        : blackberryboot.cpp
// Description : Blackberry bootstrap
//============================================================================
#if defined(__QNXNTO__)

#include "../../Logger.h"
#include "../platformgl.h"
#include "../../batterytech.h"
#include "../../render/GraphicsConfiguration.h"
#include "../../batterytech_globals.h"
#include "../../primitives.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <EGL/egl.h>
#include <screen/screen.h>
#include <sys/platform.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>
#include "../platformgeneral.h"
#include "../../Context.h"
#include <sys/keycodes.h>
#include <bps/sensor.h>
#include "BlackberryAudioGW.h"
#include <bps/paymentservice.h>
#include <iostream.h>
#include "blackberrygeneral.h"

static EGLDisplay egl_disp;
static EGLSurface egl_surf;
static EGLConfig egl_conf;
static EGLContext egl_ctx;

static screen_context_t screen_ctx;
static screen_window_t screen_win;
static screen_display_t screen_disp;
static int nbuffers = 2;

extern BlackberryAudioGW *bbAudioGW;

// this is because CLOCK_MONOTONIC is in linux/time.h but the correct file to include is sys
// only the indexer cares here.
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

/**
 * Microseconds between accelerometer reads. This is the rate at which the
 * accelerometer data will be updated from hardware. The hardware update rate
 * is set below using sensor_set_rate.
 */
static const int ACCELEROMETER_RATE = 25000;

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>

using namespace std;
using namespace BatteryTech;

void bb_swap();
int bb_init_egl(screen_context_t ctx, bool withGLES2);
void bb_terminate();
void handleScreenEvent(bps_event_t *event);

static S32 screenWidth;
static S32 screenHeight;
static bool paused;

extern void bb_handle_event(int domain, bps_event_t *event);

int main(int argc, char *argv[]) {
	int rc;
	int exit_application = 0;
	static screen_context_t screen_cxt;

	//Create a screen context that will be used to create an EGL surface to to receive libscreen events
	screen_create_context(&screen_cxt, 0);

	//Initialize BPS library
	bps_initialize();

	//Initialize IAP libraries
	paymentservice_request_events(0);           // 0 indicates that all events
	                                            // are requested
	paymentservice_set_connection_mode(false);

	// Init batterytech (so we can get the settings)
	GraphicsConfiguration *gConfig = new GraphicsConfiguration();
	gConfig->supportsHWmipmapgen = TRUE;
	gConfig->supportsShaders = TRUE;
	gConfig->supportsFBOs = TRUE;
	gConfig->supportsUVTransform = TRUE;
	gConfig->supportsVBOs = TRUE;
	btInit(gConfig, 0, 0);

	bool useGLES2 = false;
	Property *prop = btGetContext()->appProperties->get("use_shaders");
	if (prop && prop->getBoolValue()) {
		useGLES2 = true;
	}
	//Use utility code to initialize EGL for rendering with GL ES 1.1
	if (EXIT_SUCCESS != bb_init_egl(screen_cxt, useGLES2)) {
		fprintf(stderr, "bb_init_egl failed\n");
		bb_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}

	btSetScreenSize(screenWidth, screenHeight);
	//Signal BPS library that navigator and screen events will be requested
	if (BPS_SUCCESS != screen_request_events(screen_cxt)) {
		fprintf(stderr, "screen_request_events failed\n");
		bb_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}

	if (BPS_SUCCESS != navigator_request_events(0)) {
		fprintf(stderr, "navigator_request_events failed\n");
		bb_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}

	//Signal BPS library that navigator orientation is not to be locked
	if (BPS_SUCCESS != navigator_rotation_lock(false)) {
		fprintf(stderr, "navigator_rotation_lock failed\n");
		bb_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}
	// Start accelerometer
	if (sensor_is_supported(SENSOR_TYPE_ACCELEROMETER)) {
		sensor_set_rate(SENSOR_TYPE_ACCELEROMETER, ACCELEROMETER_RATE);
		sensor_set_skip_duplicates(SENSOR_TYPE_ACCELEROMETER, true);
	}
    float force_x, force_y, force_z;
	U64 lastTimeNanos = _platform_get_time_nanos();
	// main loop
	while (!exit_application) {
		//Request and process BPS next available event
		bps_event_t *event = NULL;
		rc = bps_get_event(&event, 0);
		assert(rc == BPS_SUCCESS);
		if (event) {
			int domain = bps_event_get_domain(event);
			if (domain == screen_get_domain()) {
				handleScreenEvent(event);
			} else if (domain == navigator_get_domain()) {
				unsigned int event_code = bps_event_get_code(event);
				if (NAVIGATOR_EXIT == event_code) {
					exit_application = 1;
				} else if (NAVIGATOR_WINDOW_STATE == event_code) {
					if (navigator_event_get_window_state(event) == NAVIGATOR_WINDOW_FULLSCREEN) {
						/*
						 * The application is now full screen.
						 * Resume reading the accelerometer.
						 */
						paused = false;
						if (bbAudioGW) {
							bbAudioGW->paused = false;
						}
						sensor_request_events(SENSOR_TYPE_ACCELEROMETER);
					} else if (!paused) {
						/*
						 * The application has become thumbnailed or invisible.
						 * If it is not already paused, then pause it.
						 * Otherwise, ignore the window state change.
						 */
						paused = true;
						if (bbAudioGW) {
							bbAudioGW->paused = true;
						}
						sensor_stop_events(SENSOR_TYPE_ACCELEROMETER);
					}
				}
			} else if (domain == sensor_get_domain()) {
				if (SENSOR_ACCELEROMETER_READING == bps_event_get_code(event)) {
					sensor_event_get_xyz(event, &force_x, &force_y, &force_z);
					btAccelerometerChanged(force_x, force_y, force_z);
				}
			}
			bb_handle_event(domain, event);
		}
		U64 timeNanos = _platform_get_time_nanos();
		if (!paused) {
			btUpdate(((timeNanos - lastTimeNanos) / 1000000) / 1000.0f);
			btDraw();
			eglSwapBuffers(egl_disp, egl_surf);
		}
		lastTimeNanos = timeNanos;
	}

//Stop requesting events from libscreen
	screen_stop_events(screen_cxt);

//Shut down BPS library for this process
	bps_shutdown();

//Use utility code to terminate EGL setup
	bb_terminate();

//Destroy libscreen context
	screen_destroy_context(screen_cxt);
	return 0;
}

static void bb_egl_perror(const char *msg) {
	static const char *errmsg[] = { "function succeeded",
			"EGL is not initialized, or could not be initialized, for the specified display",
			"cannot access a requested resource", "failed to allocate resources for the requested operation",
			"an unrecognized attribute or attribute value was passed in an attribute list",
			"an EGLConfig argument does not name a valid EGLConfig",
			"an EGLContext argument does not name a valid EGLContext",
			"the current surface of the calling thread is no longer valid",
			"an EGLDisplay argument does not name a valid EGLDisplay", "arguments are inconsistent",
			"an EGLNativePixmapType argument does not refer to a valid native pixmap",
			"an EGLNativeWindowType argument does not refer to a valid native window",
			"one or more argument values are invalid",
			"an EGLSurface argument does not name a valid surface configured for rendering",
			"a power management event has occurred", "unknown error code" };

	int message_index = eglGetError() - EGL_SUCCESS;

	if (message_index < 0 || message_index > 14)
		message_index = 15;

	fprintf(stderr, "%s: %s\n", msg, errmsg[message_index]);
}

int bb_init_egl(screen_context_t ctx, bool withGLES2) {
	int usage;
	int format = SCREEN_FORMAT_RGBX8888;
	EGLint interval = 1;
	int rc, num_configs;

	EGLint attrib_list[] = { EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_DEPTH_SIZE, 16, EGL_STENCIL_SIZE,
			0, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RENDERABLE_TYPE, 0, EGL_NONE };

	if (!withGLES2) {
		usage = SCREEN_USAGE_OPENGL_ES1 | SCREEN_USAGE_ROTATION;
		attrib_list[13] = EGL_OPENGL_ES_BIT;
	} else {
		usage = SCREEN_USAGE_OPENGL_ES2 | SCREEN_USAGE_ROTATION;
		attrib_list[13] = EGL_OPENGL_ES2_BIT;
	}
//Simple egl initialization
	screen_ctx = ctx;

	egl_disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (egl_disp == EGL_NO_DISPLAY) {
		bb_egl_perror("eglGetDisplay");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = eglInitialize(egl_disp, NULL, NULL);
	if (rc != EGL_TRUE) {
		bb_egl_perror("eglInitialize");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = eglBindAPI(EGL_OPENGL_ES_API);

	if (rc != EGL_TRUE) {
		bb_egl_perror("eglBindApi");
		bb_terminate();
		return EXIT_FAILURE;
	}

	if (!eglChooseConfig(egl_disp, attrib_list, &egl_conf, 1, &num_configs)) {
		bb_terminate();
		return EXIT_FAILURE;
	}

	if (withGLES2) {
		EGLint attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
		egl_ctx = eglCreateContext(egl_disp, egl_conf, EGL_NO_CONTEXT, attributes);
	} else {
		egl_ctx = eglCreateContext(egl_disp, egl_conf, EGL_NO_CONTEXT, NULL);
	}

	if (egl_ctx == EGL_NO_CONTEXT) {
		bb_egl_perror("eglCreateContext");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = screen_create_window(&screen_win, screen_ctx);
	if (rc) {
		perror("screen_create_window");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = screen_create_window_group(screen_win, btGetContext()->appProperties->get("windowed_app_name")->getValue());
	if (rc) {
		perror("screen_create_window_group");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_FORMAT, &format);
	if (rc) {
		perror("screen_set_window_property_iv(SCREEN_PROPERTY_FORMAT)");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_USAGE, &usage);
	if (rc) {
		perror("screen_set_window_property_iv(SCREEN_PROPERTY_USAGE)");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = screen_get_window_property_pv(screen_win, SCREEN_PROPERTY_DISPLAY, (void **) &screen_disp);
	if (rc) {
		perror("screen_get_window_property_pv");
		bb_terminate();
		return EXIT_FAILURE;
	}

	int screen_resolution[2];

	rc = screen_get_display_property_iv(screen_disp, SCREEN_PROPERTY_SIZE, screen_resolution);
	if (rc) {
		perror("screen_get_display_property_iv");
		bb_terminate();
		return EXIT_FAILURE;
	}

	int angle = atoi(getenv("ORIENTATION"));

	screen_display_mode_t screen_mode;
	rc = screen_get_display_property_pv(screen_disp, SCREEN_PROPERTY_MODE, (void**) &screen_mode);
	if (rc) {
		perror("screen_get_display_property_pv");
		bb_terminate();
		return EXIT_FAILURE;
	}

	int size[2];
	rc = screen_get_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
	if (rc) {
		perror("screen_get_window_property_iv");
		bb_terminate();
		return EXIT_FAILURE;
	}

	int buffer_size[2] = { size[0], size[1] };
	if (angle == 0 || angle == 180) {
		screenWidth = (S32) screen_mode.width;
		screenHeight = (S32) screen_mode.height;
	} else {
		screenWidth = (S32) screen_mode.height;
		screenHeight = (S32) screen_mode.width;
	}

	if ((angle == 0) || (angle == 180)) {
		if (((screen_mode.width > screen_mode.height) && (size[0] < size[1]))
				|| ((screen_mode.width < screen_mode.height) && (size[0] > size[1]))) {
			buffer_size[1] = size[0];
			buffer_size[0] = size[1];
		}
	} else if ((angle == 90) || (angle == 270)) {
		if (((screen_mode.width > screen_mode.height) && (size[0] > size[1]))
				|| ((screen_mode.width < screen_mode.height && size[0] < size[1]))) {
			buffer_size[1] = size[0];
			buffer_size[0] = size[1];
		}
	} else {
		fprintf(stderr, "Navigator returned an unexpected orientation angle.\n");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size);
	if (rc) {
		perror("screen_set_window_property_iv");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_ROTATION, &angle);
	if (rc) {
		perror("screen_set_window_property_iv");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = screen_create_window_buffers(screen_win, nbuffers);
	if (rc) {
		perror("screen_create_window_buffers");
		bb_terminate();
		return EXIT_FAILURE;
	}

	egl_surf = eglCreateWindowSurface(egl_disp, egl_conf, screen_win, NULL);
	if (egl_surf == EGL_NO_SURFACE) {
		bb_egl_perror("eglCreateWindowSurface");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = eglMakeCurrent(egl_disp, egl_surf, egl_surf, egl_ctx);
	if (rc != EGL_TRUE) {
		bb_egl_perror("eglMakeCurrent");
		bb_terminate();
		return EXIT_FAILURE;
	}

	rc = eglSwapInterval(egl_disp, interval);
	if (rc != EGL_TRUE) {
		bb_egl_perror("eglSwapInterval");
		bb_terminate();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void bb_terminate() {
//Typical EGL cleanup
	if (egl_disp != EGL_NO_DISPLAY) {
		eglMakeCurrent(egl_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (egl_surf != EGL_NO_SURFACE) {
			eglDestroySurface(egl_disp, egl_surf);
			egl_surf = EGL_NO_SURFACE;
		}
		if (egl_ctx != EGL_NO_CONTEXT) {
			eglDestroyContext(egl_disp, egl_ctx);
			egl_ctx = EGL_NO_CONTEXT;
		}
		if (screen_win != NULL) {
			screen_destroy_window(screen_win);
			screen_win = NULL;
		}
		eglTerminate(egl_disp);
		egl_disp = EGL_NO_DISPLAY;
	}
	eglReleaseThread();
}

void bb_swap() {
	int rc = eglSwapBuffers(egl_disp, egl_surf);
	if (rc != EGL_TRUE) {
		bb_egl_perror("eglSwapBuffers");
	}
}

void handleScreenEvent(bps_event_t *event) {
	screen_event_t screen_event = screen_event_get_event(event);

	int screen_val;
	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &screen_val);
	int pair[2];
	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION, pair);
	int touchId;
	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TOUCH_ID, &touchId);
	if (touchId >= 0 && touchId < MAX_POINTERS) {
		switch (screen_val) {
		case SCREEN_EVENT_MTOUCH_TOUCH:
			btSetPointerState(touchId, TRUE, pair[0], pair[1]);
			break;
		case SCREEN_EVENT_MTOUCH_MOVE:
			btSetPointerState(touchId, TRUE, pair[0], pair[1]);
			break;
		case SCREEN_EVENT_MTOUCH_RELEASE:
			btSetPointerState(touchId, FALSE, pair[0], pair[1]);
			break;
		case SCREEN_EVENT_KEYBOARD:
			int keyFlags;
			int keySymbol;
			screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_FLAGS, &keyFlags);
			screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_SYM, &keySymbol);
			if (keyFlags & KEY_DOWN) {
				btKeyPressed((U8) keySymbol, SKEY_NULL);
			}
			break;
		}
	}
}

#endif /* blackberry */
