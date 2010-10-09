/*
 * logger.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#include "Logger.h"
#include "platform/platformgeneral.h"

Logger::Logger() {
}

void Logger::logMsg(const char* msg) {
	_platform_log(msg);
}

Logger::~Logger() {
}
