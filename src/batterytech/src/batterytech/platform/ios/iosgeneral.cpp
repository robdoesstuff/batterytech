/*
 *  iosgeneral.cpp
 *  batterytech-ios
 *
 *  Created by Apple on 10/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE

#include "iosgeneral.h"
#include <stdlib.h>

void _platform_log(const char* message) {
}

unsigned char* _platform_load_asset(const char *filename, int *size) {
	return 0;
}

void _platform_free_asset(unsigned char *ptr) {
	if (ptr) {
		free(ptr);
	}
}

void _platform_init_sound(SoundManager *soundManager) {
}

#endif
