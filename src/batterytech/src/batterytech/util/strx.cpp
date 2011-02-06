/*
 * strx.h
 *
 *  Created on: Sep 6, 2010
 *      Author: rgreen
 */

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
