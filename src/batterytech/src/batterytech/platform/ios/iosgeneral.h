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
// Name        : iosgeneral.h
// Description : iOS platform general functions
// Usage       : Include platformgeneral.h in your application code.  Do not include this directly.
//============================================================================

#ifndef IOSGENERAL_H_
#define IOSGENERAL_H_

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE

// iphone specific functions here
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int SOCKET;
#define closesocket(socket) close(socket)

#endif /* TARGET_OS_IPHONE */

#endif /* IOSGENERAL_H_ */
