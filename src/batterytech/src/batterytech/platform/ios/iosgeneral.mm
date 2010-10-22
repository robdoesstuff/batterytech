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
#include <Foundation/Foundation.h>

void _platform_log(const char* message) {
	NSLog(@"%s", message);
}

unsigned char* _platform_load_asset(const char *filename, int *size) {
	char *data;
	char *lastDot = NULL;
	char path[strlen(filename)];
	char extension[strlen(filename)];
	lastDot = strrchr(filename, '.');
	if (lastDot) {
		int lastDotPos = lastDot - filename;
		int extLength = strlen(filename) - lastDotPos - 1;
		strncpy(path, filename, lastDotPos);
		path[lastDotPos] = '\0';
		strncpy(extension, filename + lastDotPos + 1, extLength);
		extension[extLength] = '\0';
		NSString *pathString = [NSString stringWithCString:path encoding: NSUTF8StringEncoding];
		NSString *extString = [NSString stringWithCString:extension encoding: NSUTF8StringEncoding];
		NSString *filePath = [[NSBundle mainBundle] pathForResource:pathString ofType:extString];
		const char *filePathCString = [filePath UTF8String];
		FILE *handle;
		unsigned char *data = 0;
		handle = fopen(filePathCString, "rb");
		if (!handle) {
			NSLog(@"No File Handle");		
		}
		fseek(handle, 0L, SEEK_END);
		*size = ftell(handle);
		//fseek(handle, 0L, SEEK_SET);
		rewind(handle);
		data = (unsigned char*) malloc(sizeof(unsigned char) * *size);
		if (data) {
			int bytesRead = fread(data, sizeof(unsigned char), *size, handle);
			//cout << "malloc success, " << bytesRead << " bytes read of " << *size << endl;
		}
		int error = ferror(handle);
		if (error) {
			//cout << "IO error " << error << endl;
		}
		if (feof(handle)) {
			//cout << "EOF reached " << endl;
		}
		fclose(handle);
		return data;
	}
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
