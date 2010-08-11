/*
 * logger.h
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef LOGGER_H_
#define LOGGER_H_


class logger {
public:
	logger();
	static void logMsg(const char* msg);
	virtual ~logger();
};

#define log(message) logger::logMsg(message)

#endif /* LOGGER_H_ */
