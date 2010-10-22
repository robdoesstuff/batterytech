/*
 *  iosgeneral.h
 *  batterytech-ios
 *
 *  Created by Apple on 10/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef IOSGENERAL_H_
#define IOSGENERAL_H_

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE

#include "../../sound/SoundManager.h"

void _platform_log(const char* message);
unsigned char* _platform_load_asset(const char *filename, int *size);
void _platform_free_asset(unsigned char *ptr);
void _platform_init_sound(SoundManager *soundManager);

#endif /* TARGET_OS_IPHONE */

#endif /* IOSGENERAL_H_ */