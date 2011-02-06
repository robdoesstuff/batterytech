/*
 * strx.h
 *
 *  Created on: Sep 6, 2010
 *      Author: rgreen
 */

#ifndef STRX_H_
#define STRX_H_

#include "../primitives.h"
/*
 * Determines if a string starts with another string
 */
BOOL32 strStartsWith(const char *str, const char *start);

// Copies a string into new memory of the source string's length.
char* strDuplicate(const char *str);

#endif /* STRX_H_ */
