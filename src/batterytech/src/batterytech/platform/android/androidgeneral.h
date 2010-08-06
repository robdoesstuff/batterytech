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
#include <jni.h>
#include <stdlib.h>

void _platform_log(const char* message);
unsigned char* _platform_load_asset(const char *filename, int *size);
void _platform_free_asset(unsigned char *ptr);


#endif /* ANDROID_NDK */

#endif /* ANDROIDGENERAL_H_ */
