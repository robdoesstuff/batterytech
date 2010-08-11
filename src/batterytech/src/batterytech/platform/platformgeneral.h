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

#endif /* PLATFORMGENERAL_H_ */
