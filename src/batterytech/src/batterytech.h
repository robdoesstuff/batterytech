/*
 * batterytech.h
 *
 *  Created on: Jul 28, 2010
 *      Author: rgreen
 */

#ifndef BATTERYTECH_H_
#define BATTERYTECH_H_

#define DEBUG 0

#ifdef ANDROID_NDK
	#include "platform/android/importgl.h"
	#include "platform/android/androidtypes.h"
	#include "platform/android/androidgeneral.h"
#endif /* ANDROID_NDK */

#ifdef _WIN32
	#include <gl/gl.h>
	#include <gl/glu.h>
	#include "platform/win32/win32types.h"
	#include "platform/win32/win32general.h"
#endif /* _WIN32 */

#include <stdio.h>
#include "logger.h"
#include "stb_image.h"

#define log(message) logger::log(message)

void btInit();
void btUpdate(float delta);
void btDraw();

#endif /* BATTERYTECH_H_ */

