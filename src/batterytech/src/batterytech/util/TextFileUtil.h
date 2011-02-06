/*
 * TextFileUtil.h
 *
 *  Created on: Dec 1, 2010
 *      Author: rgreen
 */

#ifndef TEXTFILEUTIL_H_
#define TEXTFILEUTIL_H_
#include <stdio.h>
#include "../primitives.h"

class TextFileUtil {
public:
	TextFileUtil();
	virtual ~TextFileUtil();
	/* Reads a line of text.  Ignores blank lines.  Returns FALSE if EOF */
	static BOOL32 readLine(char *buf, FILE *file);
	/* Reads a line of text.  Starts writing to buf at bufOffset, Ignores blank lines.  Returns FALSE if EOF */
	static BOOL32 readLine(char *buf, S32 bufOffset, FILE *file);
	/* Reads a line of text.  Ignores blank lines.  Returns FALSE if EOB
	 * lineBuf is output, buffer is input, startPos should be 0 and will be set at last read char + 1 */
	static BOOL32 readLine(char *lineBuf, const char *buffer, S32 *startPos);
};

#endif /* TEXTFILEUTIL_H_ */
