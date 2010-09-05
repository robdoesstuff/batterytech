/*
 * batterytech.h
 *
 * the basic entry functions to the engine - all functions are either lifecycle or IO.
 *
 *  Created on: Jul 28, 2010
 *      Author: rgreen
 */

#ifndef BATTERYTECH_H_
#define BATTERYTECH_H_

#include "primitives.h"

/*
 * Called once to initialize the engine
 */
void btInit(S32 width, S32 height);

/*
 * Called when the screen size has changed
 */
void btSetScreenSize(S32 width, S32 height);

/*
 * Sets the state of a pointer
 */
void btSetPointerState(S32 pointerId, BOOL32 down, S32 x, S32 y);

/*
 * Called as often as possible to update the state of the engine
 *
 * delta is in seconds
 */
void btUpdate(F32 delta);

/**
 * Draws the world
 */
void btDraw();

/**
 * Suspends the application (should always be called before release to give a chance to autosave, etc)
 */
void btSuspend();

/**
 * Resumes the application - always called after init but before update or draw
 */
void btResume();

/**
 * Releases (finalizes) the application
 */
void btRelease();

#endif /* BATTERYTECH_H_ */

