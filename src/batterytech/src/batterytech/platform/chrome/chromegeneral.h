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
// Name        : chromegeneral.h
// Description : chrome platform general functions
// Usage       : Include platformgeneral.h in your application code.  Do not include this directly.
//============================================================================

#ifndef CHROMEGENERAL_H_
#define CHROMEGENERAL_H_

#ifdef CHROME

#include <string>

typedef int SOCKET;

struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
};


// IPv4 AF_INET sockets:

struct in_addr {
    unsigned long s_addr;          // load with inet_pton()
};
struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET, AF_INET6
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};



// IPv6 AF_INET6 sockets:

struct in6_addr {
    unsigned char   s6_addr[16];   // load with inet_pton()
};
struct sockaddr_in6 {
    unsigned short       sin6_family;   // address family, AF_INET6
    unsigned short       sin6_port;     // port number, Network Byte Order
    unsigned int       sin6_flowinfo; // IPv6 flow information
    struct in6_addr sin6_addr;     // IPv6 address
    unsigned int       sin6_scope_id; // Scope ID
};


struct addrinfo {
	int             ai_flags;
	int             ai_family;
	int             ai_socktype;
	int             ai_protocol;
	size_t			ai_addrlen;
	char            *ai_canonname;
	struct sockaddr  *ai_addr;
	struct addrinfo  *ai_next;
};

typedef unsigned int sa_family_t;

/*
 *  Desired design of maximum size and alignment.
 */
#define _SS_MAXSIZE 128
    /* Implementation-defined maximum size. */
#define _SS_ALIGNSIZE (sizeof(int64_t))
    /* Implementation-defined desired alignment. */
#define _SS_PAD1SIZE (_SS_ALIGNSIZE - sizeof(sa_family_t))
#define _SS_PAD2SIZE (_SS_MAXSIZE - (sizeof(sa_family_t)+ \
                      _SS_PAD1SIZE + _SS_ALIGNSIZE))
struct sockaddr_storage {
    sa_family_t  ss_family;  /* Address family. */
/*
 *  Following fields are implementation-defined.
 */
    char _ss_pad1[_SS_PAD1SIZE];
        /* 6-byte pad; this is to make implementation-defined
           pad up to alignment field that follows explicit in
           the data structure. */
    int64_t _ss_align;  /* Field to force desired structure
                           storage alignment. */
    char _ss_pad2[_SS_PAD2SIZE];
        /* 112-byte pad to achieve desired size,
           _SS_MAXSIZE value minus size of ss_family
           __ss_pad1, __ss_align fields is 112. */
};

#define AI_PASSIVE     1
#define AI_CANONNAME   2
#define EAI_ADDRFAMILY -1
#define EAI_AGAIN      -2
#define EAI_BADFLAGS   -3
#define EAI_FAIL       -4
#define EAI_FAMILY     -5
#define EAI_MEMORY     -6
#define EAI_NODATA     -7
#define EAI_NONAME     -8
#define EAI_SERVICE    -9
#define EAI_SOCKTYPE   -10
#define EAI_SYSTEM     -11


#define AF_UNSPEC 0
#define AF_INET 2
#define AF_INET6 23
#define AF_IRDA 26
#define AF_BTH 32

/*
 * Types
 */
#define	SOCK_STREAM	1		/* stream socket */
#define	SOCK_DGRAM	2		/* datagram socket */
#define	SOCK_RAW	3		/* raw-protocol interface */
#define	SOCK_RDM	4		/* reliably-delivered message */
#define	SOCK_SEQPACKET	5		/* sequenced packet stream */

/*
 * Option flags per-socket.
 */
#define	SO_DEBUG	0x0001		/* turn on debugging info recording */
#define	SO_ACCEPTCONN	0x0002		/* socket has had listen() */
#define	SO_REUSEADDR	0x0004		/* allow local address reuse */
#define	SO_KEEPALIVE	0x0008		/* keep connections alive */
#define	SO_DONTROUTE	0x0010		/* just use interface addresses */
#define	SO_BROADCAST	0x0020		/* permit sending of broadcast msgs */
#define	SO_USELOOPBACK	0x0040		/* bypass hardware when possible */
#define	SO_LINGER	0x0080		/* linger on close if data present */
#define	SO_OOBINLINE	0x0100		/* leave received OOB data in line */

/*
 * Additional options, not kept in so_options.
 */
#define SO_SNDBUF	0x1001		/* send buffer size */
#define SO_RCVBUF	0x1002		/* receive buffer size */
#define SO_SNDLOWAT	0x1003		/* send low-water mark */
#define SO_RCVLOWAT	0x1004		/* receive low-water mark */
#define SO_SNDTIMEO	0x1005		/* send timeout */
#define SO_RCVTIMEO	0x1006		/* receive timeout */
#define	SO_ERROR	0x1007		/* get error status and clear */
#define	SO_TYPE		0x1008		/* get socket type */

#define	SOL_SOCKET	0xffff		/* options for socket level */

#define INET_ADDRSTRLEN        16
#define INET6_ADDRSTRLEN       46

typedef int socklen_t;

int accept(int socket, struct sockaddr *address, socklen_t *address_len);
int bind(int socket, const struct sockaddr *address, socklen_t address_len);
int connect(int socket, const struct sockaddr *address, socklen_t address_len);
int getpeername(int socket, struct sockaddr *address, socklen_t *address_len);
int getsockname(int socket, struct sockaddr *address, socklen_t *address_len);
int getsockopt(int socket, int level, int option_name, void *option_value,
		socklen_t *option_len);
int listen(int socket, int backlog);
ssize_t recv(int socket, void *buffer, size_t length, int flags);
ssize_t recvfrom(int socket, void *buffer, size_t length, int flags,
		struct sockaddr *address, socklen_t *address_len);
ssize_t recvmsg(int socket, struct msghdr *message, int flags);
ssize_t send(int socket, const void *message, size_t length, int flags);
ssize_t sendmsg(int socket, const struct msghdr *message, int flags);
ssize_t sendto(int socket, const void *message, size_t length, int flags,
		const struct sockaddr *dest_addr, socklen_t dest_len);
int setsockopt(int socket, int level, int option_name,
		const void *option_value, socklen_t option_len);
int shutdown(int socket, int how);
int socket(int domain, int type, int protocol);
int socketpair(int domain, int type, int protocol, int socket_vector[2]);

int getaddrinfo(const char *hostname, const char *service,
		const struct addrinfo *hints, struct addrinfo **res);
int getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host,
		size_t hostlen, char *serv, size_t servlen, int flags);
void freeaddrinfo(struct addrinfo *ai);
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
int inet_pton(int af, const char *src, void *dst);

#define closesocket close

// chrome specific functions here

#endif /* chrome */

#endif /* CHROMEGENERAL_H_ */
