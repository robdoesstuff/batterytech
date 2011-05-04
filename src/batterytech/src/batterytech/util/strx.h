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

#endif /* STRX_H_ */
