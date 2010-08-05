/*
 * logger.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#include "batterytech.h"
#include "logger.h"

logger::logger() {
}

void logger::log(const char* msg) {
	_platform_log(msg);
}

logger::~logger() {
}
