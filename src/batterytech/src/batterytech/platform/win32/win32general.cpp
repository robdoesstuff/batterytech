/*
 * win32general.cpp
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef WIN32GENERAL_CPP_
#define WIN32GENERAL_CPP_
#ifdef _WIN32

#include "win32general.h"

using namespace std;

void _platform_log(const char* message) {
	cout << message << endl;
}

#endif /* _WIN32 */
#endif /* WIN32GENERAL_CPP_ */
