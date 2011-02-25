/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : strx.cpp
// Description : String extension functions
//============================================================================

#ifndef STRX_H_
#define STRX_H_

#include "../primitives.h"
#include <string.h>

/*
 * Determines if a string starts with another string
 */
BOOL32 strStartsWith(const char *str, const char *start) {
	int strLength = strlen(str);
	int startLength = strlen(start);
	if (strLength < startLength) {
		return FALSE;
	}
	int i;
	for (i = 0; i < startLength; i++) {
		if (str[i] != start[i]) {
			return FALSE;
		}
	}
	return TRUE;
}

// Copies a string into new memory of the source string's length.
char* strDuplicate(const char *str) {
	if (str) {
		S32 textLength = strlen(str);
		char *newStr = new char[textLength + 1];
		strcpy(newStr, str);
		return newStr;
	} else {
		return NULL;
	}
}

#endif /* STRX_H_ */
