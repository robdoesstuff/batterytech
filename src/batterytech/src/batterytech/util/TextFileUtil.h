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
// Name        : TextFileUtil.h
// Description : Utility to help with reading lines from text files
//============================================================================

#ifndef TEXTFILEUTIL_H_
#define TEXTFILEUTIL_H_
#include <stdio.h>
#include "../primitives.h"

namespace BatteryTech {

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

}

#endif /* TEXTFILEUTIL_H_ */
