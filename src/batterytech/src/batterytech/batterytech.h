/*
 * batterytech.h
 *
 *  Created on: Jul 28, 2010
 *      Author: rgreen
 */

#ifndef BATTERYTECH_H_
#define BATTERYTECH_H_

#ifdef ANDROID_NDK
	#include "platform/android/importgl.h"
#endif /* ANDROID_NDK */

#ifdef _WIN32
	#include <gl/gl.h>
	#include <gl/glu.h>
#endif /* _WIN32 */

#define log(message) logger::log(message)

void btInit();
void btUpdate(float delta);
void btDraw();
void btRelease();

#endif /* BATTERYTECH_H_ */

