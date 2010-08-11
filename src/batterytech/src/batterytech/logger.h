/*
 * logger.h
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#define log(message) logger::log(message)

class logger {
public:
	logger();
	static void log(const char* msg);
	virtual ~logger();
};

#endif /* LOGGER_H_ */
