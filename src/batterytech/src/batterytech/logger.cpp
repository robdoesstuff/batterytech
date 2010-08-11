/*
 * logger.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#include "logger.h"
#include "platform/platformgeneral.h"

logger::logger() {
}

void logger::logMsg(const char* msg) {
	_platform_log(msg);
}

logger::~logger() {
}
