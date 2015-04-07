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
// Name        : primitives.h
// Description : Cross-platform primitive declarations
// Usage       : Always include this and use instead of using primitives directly.
//============================================================================

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

/**
 * \file
 * \ingroup Primitives
 * \brief Cross-platform Primitive definitions.  Include \ref batterytech/primitives.h and not a platform-specific implementation to use.
 *
 * Cross-platform primitives are a safe way to ensure that any structs or data pushed through network or through disk will
 * be consistent across different definitions of the raw C primitive types.  It doesn't help with endianness but currently all supported platforms use
 * little endian so it shouldn't be an issue until we support a big endian platform.
 *
 * Example:
 * \code
 * BOOL32 found = FALSE;
 * for (S32 i = 0; i < numObjects; i++) {
 *     if (matches(objects[i])) {
 *         found = TRUE;
 *         break;
 *     }
 * }
 * \endcode
 *
 * @{
 */

#ifdef DOXYGEN
/** \typedef S8
 * \brief A signed 8 bit integer
 *
 * A signed 8 bit integer
 */
typedef signed char S8;
/** \typedef U8
 * \brief An unsigned 8 bit integer
 *
 * An unsigned 8 bit integer
 */
typedef unsigned char U8;
/** \typedef S16
 * \brief A signed 16 bit integer
 *
 * A signed 16 bit integer
 */
typedef signed short S16;
/** \typedef U16
 * \brief An unsigned 16 bit integer
 *
 * An unsigned 16 bit integer
 */
typedef unsigned short U16;
/** \typedef S32
 * \brief A signed 32 bit integer
 *
 * A signed 32 bit integer
 */
typedef signed int S32;
/** \typedef U32
 * \brief An unsigned 32 bit integer
 *
 * An unsigned 32 bit integer
 */
typedef unsigned int U32;
/** \typedef S64
 * \brief A signed 64 bit integer
 *
 * A signed 64 bit integer
 */
typedef signed long long S64;
/** \typedef U64
 * \brief An unsigned 64 bit integer
 *
 * An unsigned 64 bit integer
 */
typedef unsigned long long U64;

/** \typedef F32
 * \brief A 32 bit floating point number
 *
 * A 32 bit floating point number
 */
typedef float F32;
/** \typedef F64
 * \brief A 64 bit floating point number
 *
 * A 64 bit floating point number
 */
typedef double F64;

/** \typedef BYTE8
 * \brief An unsigned byte
 *
 * An unsigned byte
 */
typedef unsigned char BYTE8;
/** \typedef BOOL32
 * \brief A 32 bit boolean value
 *
 * A 32 bit boolean value
 */
typedef U32 BOOL32;

/** \def TRUE
 * \brief True
 *
 * True
 */
#ifndef TRUE
#define TRUE 1
#endif

/** \def FALSE
 * \brief False
 *
 * False
 */
#ifndef FALSE
#define FALSE 0
#endif
#endif

/** \def NULL
 * \brief Standard null, suitable for pointers
 *
 * Standard null, suitable for pointers
 */
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

#if defined(__QNXNTO__)
#include "platform/blackberry/blackberrytypes.h"
#endif

#if defined(linux) and not defined(ANDROID_NDK)
	#include "platform/linux/linuxtypes.h"
#endif

#if CHROME
	#include "platform/chrome/chrometypes.h"
#endif

// special key definitions (not really primitives but this is a convenient header to store them in)
namespace BatteryTech {
	/** \brief Special Key Definitions
	 *
	 * These keys are available on many devices and are mapped to the specific device's key.
	 */
	enum SpecialKey { SKEY_NULL, SKEY_BACK, SKEY_MENU, SKEY_SEARCH, SKEY_HOME, SKEY_CAMERA, SKEY_WHEELLEFT, SKEY_WHEELRIGHT, SKEY_WHEELUP, SKEY_WHEELDOWN };
}

/**@}*/

#endif /* PRIMITIVES_H_ */
