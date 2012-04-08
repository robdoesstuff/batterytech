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
// Name        : strx.h
// Description : String extension functions
//============================================================================

#ifndef STRX_H_
#define STRX_H_

#include "../primitives.h"
/*
 * Determines if a string starts with another string
 */
BOOL32 strStartsWith(const char *str, const char *start);

// Copies a string into new memory of the source string's length. Note - uses "new" so you must "delete" instead of free
char* strDuplicate(const char *str);

// Determines string equality, wrapper for strcmp
BOOL32 strEquals(const char *s1, const char *s2);

// Determines if needle exists in haystack, wrapper for strstr
BOOL32 strContains(const char *haystack, const char *needle);

// Gets the encoded length of this string
S32 strlenUTF8(const char *str);

// Converts a UTF-8 byte sequence to a unicode code point
S32 strToUnicode(const char *str, S32 *bytesConsumed);

// Converts an array of UTF-8 byte sequences to a unicode code point array
// returns number of unicode code points in unicodeArray (<= maxLen)
S32 strnUTF8ToUnicodeArray(const char *str, S32 *unicodeArray, const S32 maxLen);

#endif /* STRX_H_ */
