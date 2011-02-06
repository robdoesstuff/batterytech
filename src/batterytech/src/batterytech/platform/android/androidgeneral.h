/*
 * androidgeneral.h
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef ANDROIDGENERAL_H_
#define ANDROIDGENERAL_H_

#ifdef ANDROID_NDK

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int SOCKET;
#define closesocket(socket) close(socket)

// android specific functions here

#endif /* ANDROID_NDK */

#endif /* ANDROIDGENERAL_H_ */
