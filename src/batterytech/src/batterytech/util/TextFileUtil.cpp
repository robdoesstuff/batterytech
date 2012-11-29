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
// Name        : TextFileUtil.cpp
// Description : Utility to help with reading lines from text files
//============================================================================

#include "TextFileUtil.h"
#include <stdio.h>

namespace BatteryTech {

	TextFileUtil::TextFileUtil() {
	}

	TextFileUtil::~TextFileUtil() {
	}

	BOOL32 TextFileUtil::readLine(char *buf, S32 bufOffset, FILE *file) {
		S32 pos = -1;
		int c;
		while (true) {
			c = fgetc(file);
			if (c == '\r') {
				// ignore carriage returns (only works when reading in binary mode
				continue;
			}
			if ((c == '\n' || c == ' ' || c == '\t' || c == '\f') && pos == -1) {
				// ignore empty lines and leading white space
				continue;
			}
			if (c == '\n' || c == EOF) {
				buf[++pos + bufOffset] = (char)NULL;
				return (c != EOF);
			}
			buf[++pos + bufOffset] = c;
		}
	}

	BOOL32 TextFileUtil::readLine(char *buf, FILE *file) {
		return readLine(buf, 0, file);
	}

	BOOL32 TextFileUtil::readLine(char *lineBuf, const char *buffer, S32 *startPos) {
		// pos should start at 0, will end at last read + 1
		S32 pos = -1;
		char c;
		while (true) {
			c = buffer[*startPos];
			if (c == '\r') {
				// ignore carriage returns (only works when reading in binary mode
				(*startPos)++;
				continue;
			}
			if ((c == '\n' || c == ' ' || c == '\t' || c == '\f') && pos == -1) {
				// ignore empty lines and leading white space
				(*startPos)++;
				continue;
			}
			if (c == '\n' || c == '\0') {
				lineBuf[++pos] = '\0';
				if (c != '\0') {
					(*startPos)++;
					return TRUE;
				}
				return FALSE;
			}
			lineBuf[++pos] = c;
			(*startPos)++;
		}
	}

}
