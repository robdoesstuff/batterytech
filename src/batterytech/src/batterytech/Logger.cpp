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
// Name        : Logger.cpp
// Description : Logging Facility
//============================================================================

#include "Logger.h"
#include "platform/platformgeneral.h"
#include <stdio.h>
#include <string.h>

// static initialization
FILE *file = NULL;

Logger::Logger() {
}

void Logger::logMsg(const char* msg) {
	_platform_log(msg);
	if (file) {
		fwrite(msg, sizeof(char), strlen(msg), file);
		fwrite("\n", sizeof(char), strlen("\n"), file);
		fflush(file);
	}
}

void Logger::useFileOutput(const char* filename) {
	char convertedFilename[1024];
	if (filename) {
		_platform_convert_path(filename, convertedFilename);
		char buf[1024];
		sprintf(buf, "Logging output to %s", convertedFilename);
		_platform_log(buf);
	} else {
		_platform_log("No longer logging messages to a file.");
	}
	if (file) {
		fclose(file);
	}
	if (filename) {
		file = fopen(convertedFilename, "w");
	} else {
		file = NULL;
	}
	if (filename && !file) {
		char buf[1024];
		sprintf(buf, "Unable to create or open file for writes: %s", convertedFilename);
		_platform_log(buf);
	}
}

Logger::~Logger() {
}
