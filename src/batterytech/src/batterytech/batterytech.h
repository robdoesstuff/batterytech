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
// Name        : batterytech.h
// Description : The basic entry functions to the platform - all functions are either lifecycle, update or IO.
//============================================================================

#ifndef BATTERYTECH_H_
#define BATTERYTECH_H_

#include "render/GraphicsConfiguration.h"
#include "primitives.h"

namespace BatteryTech { class Context; }

/*
 * Called once to initialize the engine
 */
void btInit(BatteryTech::GraphicsConfiguration *graphicsConfig, S32 width, S32 height);

/*
 * Called when the screen size has changed
 */
void btSetScreenSize(S32 width, S32 height);

/*
 * Sets the state of a pointer
 */
void btSetPointerState(S32 pointerId, BOOL32 down, S32 x, S32 y);

/*
 * Sets that a key has gone up
 */
void btKeyUp(U8 key, BatteryTech::SpecialKey sKey);

/*
 * Sets that a key is pressed down
 */
void btKeyDown(U8 key, BatteryTech::SpecialKey sKey);

/*
 * Sets that a key has been pressed
 */
void btKeyPressed(U8 key, BatteryTech::SpecialKey sKey);

/*
 * Called as often as possible to update the state of the engine
 *
 * delta is in seconds
 */
void btUpdate(F32 delta);

/**
 * Called when new accelerometer data has arrived
 */
void btAccelerometerChanged(F32 x, F32 y, F32 z);

/**
 * Draws the world
 */
void btDraw();

/**
 * Suspends the application (should always be called before release to give a chance to autosave, etc)
 */
void btSuspend();

/**
 * Notifies our app that the graphics context has been lost (reload is necessary)
 */
void btSetGraphicsContextLost(BOOL32 wasLost);

/**
 * Resumes the application - always called after init but before update or draw
 */
void btResume();

/**
 * Releases (finalizes) the application
 */
void btRelease();

/**
 * Callback from host OS (generally has something to do with an async callback from an earlier hook like IAP)
 *
 * Does not block but may not register callback if queue is full
 *
 * @return TRUE if callback was queued, FALSE if queue is full
 */
BOOL32 btCallback(const char *data);

/**
 * Clears current tick delta and any other values stored for smoothing
 *
 * Useful after a big single-frame operation like loading
 */
void btClearTickDeltas();

/**
 * Gets the static instance of the application context
 */
BatteryTech::Context* btGetContext();

#endif /* BATTERYTECH_H_ */

