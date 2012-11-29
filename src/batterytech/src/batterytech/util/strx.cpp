/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
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

BOOL32 strEndsWith(const char *str, const char *end) {
	int strLength = strlen(str);
	int endLength = strlen(end);
	if (strLength < endLength) {
		return FALSE;
	}
	int j = 0;
	for (int i = strLength-endLength; i < strLength; i++) {
		if (str[i] != end[j++]) {
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

BOOL32 strEquals(const char *s1, const char *s2) {
	if(strcmp(s1, s2) == 0) {
		return TRUE;
	}
	return FALSE;
}

BOOL32 strContains(const char *haystack, const char *needle) {
	if(strstr(haystack, needle) == NULL) {
		return FALSE;
	}
	return TRUE;
}


char* strTrim(char *str, S32 *length) {
	S32 strLength = strlen(str);
	char *retStr = str;
	for (S32 i = 0; i < strLength; i++) {
		if (str[i] != ' ' && str[i] != '\t') {
			retStr = &str[i];
			break;
		}
	}
	for (S32 i = strLength-1; i > -1; i--) {
		S32 c = str[i];
		if (c != ' ' && c != '\t') {
			*length = i+1 - (retStr-str);
			break;
		}
	}
	return retStr;
}

// Gets the encoded length of this string
S32 strlenUTF8(const char *str) {
	S32 chars = 0;
	char c = *str;
	while (c) {
		// check for 10xxxxxx - is not first byte of sequence!
		// c should always start on the first char of a sequence.
		if ((c & 0x80) == 0) {
			// lower ASCII
			chars++;
			c++;
		} else if ((c & 0xE0) == 0xC0) {
			// 2 byte sequence
			chars++;
			c+=2;
		} else if ((c & 0xF0) == 0xE0) {
			// 3 byte sequence
			chars++;
			c+=3;
		} else if ((c & 0xF8) == 0xF0) {
			// 4 byte sequence
			chars++;
			c+=4;
		} else if ((c & 0x80)) {
			// sequence char, push forward one to (hopefully) get to the next start char
			c++;
		}
	}
	return chars;
}

// Converts a UTF-8 byte sequence to a unicode code point, returns -1 if invalid sequence
S32 strToUnicode(const char *str, S32 *bytesConsumed) {
	if ((*str & 0x80) == 0) {
		// lower ASCII
		*bytesConsumed = 1;
		return (S32)*str;
	} else if ((*str & 0xE0) == 0xC0) {
		// 2 byte sequence
		if ((*(str+1) & 0xC0) != 0x80) {
			// char[1] is not a sequence char
			return -1;
		}
		S32 c = ((*str & 0x1F) << 6) | (*(str+1) & 0x3F);
		if (c < 0x80) {
			// unicode value total is low-ascii, that's wrong
			return -1;
		}
		*bytesConsumed = 2;
		return c;
	} else if ((*str & 0xF0) == 0xE0) {
		// 3 byte sequence
		if ((*(str+1) & 0xC0) != 0x80 || (*(str+2) & 0xC0) != 0x80) {
			// char[1] or char[2] is not a sequence char
			return -1;
		}
		S32 c = ((*str & 0xF) << 12) | ((*(str+1) & 0x3F) << 6) | (*(str+2) & 0x3F);
		if (c < 0x80) {
			// unicode value total is low-ascii, that's wrong
			return -1;
		}
		// UTF-16 surrogates should never appear in UTF-8 data.
		if (c >= 0xD800 && c <= 0xDFFF) {
			return -1;
		}
		*bytesConsumed = 3;
		return c;
	} else if ((*str & 0xF8) == 0xF0) {
		// 4 byte sequence
		if ((*(str+1) & 0xC0) != 0x80 || (*(str+2) & 0xC0) != 0x80 || (*(str+3) & 0xC0) != 0x80) {
			// char[1] or char[2] or char[3] is not a sequence char
			return -1;
		}
		S32 c = ((*str & 0x7) << 18) | ((*(str+1) & 0x3F) << 12) | ((*(str+2) & 0x3F) << 6) | (*(str+3) & 0x3F);
		if (c < 0x80) {
			// unicode value total is low-ascii, that's wrong
			return -1;
		}
		if (c < 0x10000 || c > 0x10FFFF) {
			// invalid value check
			return -1;
		}
		*bytesConsumed = 4;
		return c;
	} else if ((*str & 0x80)) {
		// sequence char, push forward one to (hopefully) get to the next start char
		// invalid start char
		*bytesConsumed = 1;
		return -1;
	}
	*bytesConsumed = 0;
	return -1;
}

S32 strnUTF8ToUnicodeArray(const char *str, S32 *unicodeArray, const S32 maxLen) {
	const char *c = str;
	S32 arrayIdx = 0;
	S32 bytesConsumed = 0;
	while (*c) {
		S32 unicode = strToUnicode(c, &bytesConsumed);
		c += bytesConsumed;
		if (unicode > -1) {
			unicodeArray[arrayIdx] = unicode;
			arrayIdx++;
			if (arrayIdx == maxLen) {
				return arrayIdx;
			}
		}
	}
	return arrayIdx;
}

U32 strHash(const char *str) {
    // This is Jenkin's one-at-a-time hash function
    U32 hash, i, len;
	len = strlen(str);
	for(hash = i = 0; i < len; ++i) {
		hash += str[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
    return hash;
}

#endif /* STRX_H_ */
