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
// Name        : linuxgeneral.h
// Description : linux platform general functions
// Usage       : Include platformgeneral.h in your application code.  Do not include this directly.
//============================================================================

#ifndef LINUXGENERAL_H_
#define LINUXGENERAL_H_

#if defined(linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int SOCKET;
#define closesocket(socket) close(socket)

// linux specific functions here

#endif /* linux */

#endif /* LINUXGENERAL_H_ */
