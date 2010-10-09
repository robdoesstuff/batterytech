/*
 * Logger.h
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef LOGGER_H_
#define LOGGER_H_


class Logger {
public:
	Logger();
	static void logMsg(const char* msg);
	virtual ~Logger();
};

#define log(message) Logger::logMsg(message)

#endif /* LOGGER_H_ */
