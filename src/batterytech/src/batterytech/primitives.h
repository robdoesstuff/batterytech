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
// Name        : primitives.h
// Description : Cross-platform primitive declarations
//============================================================================

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

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
#if TARGET_OS_MAC
	#include "platform/osx/osxtypes.h"
#endif /* TARGET_OS_MAC */
#endif /* PRIMITIVES_H_ */
