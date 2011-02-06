/*
 *  iosgeneral.h
 *  batterytech-ios
 *
 *  Created by Apple on 10/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

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
