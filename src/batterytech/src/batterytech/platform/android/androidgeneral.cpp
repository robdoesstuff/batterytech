/*
 * androidgeneral.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef WIN32GENERAL_CPP_
#define WIN32GENERAL_CPP_

#ifdef ANDROID_NDK

#include "androidgeneral.h"

void _platform_log(const char* message) {
	__android_log_print(ANDROID_LOG_DEBUG, "BatteryTech", message);
}

#endif /* ANDROID_NDK */

#endif /* WIN32GENERAL_CPP_ */
