/*
 * win32general.h
 *
 *  Created on: Jul 29, 2010
 *      Author: rgreen
 */

#ifndef WIN32GENERAL_H_
#define WIN32GENERAL_H_
#ifdef _WIN32

// minimum target version is WindowsXP to get modern networking
#undef WINVER
#define WINVER 0x0501
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>

#undef errno
#define errno WSAGetLastError()

// don't know why this isn't defined in winsock2.. should be!
#define EWOULDBLOCK	WSAEWOULDBLOCK
#define EINPROGRESS WSAEINPROGRESS
#define EALREADY    WSAEALREADY
#define EISCONN     WSAEISCONN
#define EINTR       WSAEINTR
#define EAGAIN      WSAEAGAIN
#define ENOTCONN    WSAENOTCONN
#define ECONNRESET  WSAECONNRESET
#define ETIMEDOUT   WSAETIMEDOUT
#define ENOTSOCK	WSAENOTSOCK
#define EOPNOTSUPP	WSAEOPNOTSUPP
#define ENOBUFS		WSAENOBUFS

void _platform_push_sound(unsigned char *pcm);

const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
#ifdef __MINGW32__
	int inet_pton(int af, const char *src, void *dst);
#endif

#ifndef __MINGW32__
	#include <stdlib.h>
	#include <io.h>
		// replacing unistd.h
	#define srandom srand
	#define random rand

	#define F_OK 0
	#define W_OK 2
	#define R_OK 4

	#define access _access
	#define ftruncate _chsize

	#define ssize_t int
#endif

#endif /* _WIN32 */
#endif /* WIN32GENERAL_H_ */
