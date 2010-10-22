/*
 * platformgeneral.h
 *
 *  Created on: Aug 11, 2010
 *      Author: rgreen
 */

#ifndef PLATFORMGENERAL_H_
#define PLATFORMGENERAL_H_

#ifdef ANDROID_NDK
	#include "android/androidgeneral.h"
#endif /* ANDROID_NDK */

#ifdef _WIN32
	#include "win32/win32general.h"
#endif /* _WIN32 */

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE
	#include "ios/iosgeneral.h"
#endif /* IOS */

#endif /* PLATFORMGENERAL_H_ */
