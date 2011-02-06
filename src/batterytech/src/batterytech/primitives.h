/*
 * primitives.h
 *
 *  Created on: Aug 11, 2010
 *      Author: rgreen
 */

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#define BTDEBUG 0

#ifndef NULL
#define NULL 0
#endif

#ifdef ANDROID_NDK
	#include "platform/android/androidtypes.h"
#endif /* ANDROID_NDK */

#ifdef _WIN32
	#include "platform/win32/win32types.h"
#endif /* _WIN32 */

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE
	#include "platform/ios/iostypes.h"
#endif /* TARGET_OS_IPHONE */

#endif /* PRIMITIVES_H_ */
