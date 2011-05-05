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
// Usage       : Always include this and use instead of using primitives directly.
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
#if TARGET_OS_MAC && !TARGET_OS_IPHONE
	#include "platform/osx/osxtypes.h"
#endif /* TARGET_OS_MAC */

// special key definitions (not really primitives but this is a convenient header to store them in)
namespace BatteryTech {
	enum SpecialKey { SKEY_NULL, SKEY_BACK, SKEY_MENU, SKEY_SEARCH, SKEY_HOME, SKEY_CAMERA, SKEY_WHEELLEFT, SKEY_WHEELRIGHT, SKEY_WHEELUP, SKEY_WHEELDOWN };
}

#endif /* PRIMITIVES_H_ */
