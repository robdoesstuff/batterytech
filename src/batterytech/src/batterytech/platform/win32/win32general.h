/*
 * win32general.h
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef WIN32GENERAL_H_
#define WIN32GENERAL_H_
#ifdef _WIN32

#include <iostream>
#include <windows.h>
#include "../../sound/SoundManager.h"

void _platform_log(const char *message);

unsigned char* _platform_load_asset(const char *filename, int *size);
void _platform_free_asset(unsigned char *ptr);

void _platform_init_sound(SoundManager *soundManager);
void _platform_push_sound(unsigned char *pcm);

#define glOrthof(left,right,bottom,top,near,far) glOrtho(left,right,bottom,top,near,far)

#endif /* _WIN32 */
#endif /* WIN32GENERAL_H_ */
