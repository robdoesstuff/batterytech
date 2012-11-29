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
// Name        : Logger.h
// Description : Logging Facility
//             : Very simple for now.  Will be expanded upon later.
//============================================================================

#ifndef LOGGER_H_
#define LOGGER_H_


class Logger {
public:
	Logger();
	static void logMsg(const char* msg);
	static void useFileOutput(const char* filename);
	virtual ~Logger();
};

#define logmsg(message) Logger::logMsg(message)

#endif /* LOGGER_H_ */
