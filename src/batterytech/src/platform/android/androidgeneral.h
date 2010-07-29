/*
 * androidgeneral.h
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef ANDROIDGENERAL_H_
#define ANDROIDGENERAL_H_

#ifdef ANDROID_NDK

#include <android/log.h>
void _platform_log(const char* message);


#endif /* ANDROID_NDK */

#endif /* ANDROIDGENERAL_H_ */
