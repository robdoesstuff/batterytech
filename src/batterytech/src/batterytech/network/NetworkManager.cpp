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
// Name        : NetworkManager.cpp
// Description : Established connections to clients using UDP-based network discovery, sends TCP messages to clients.
// Usage       : startHosting() will start a server and advertise the server to potential clients.
//             : findHost() will find a host as a client
//             : sendMessage() will send a message to the server or to a client
//             : createMessage() will get you a message instance (do not instance your own) from the pool.
//             : All messages must be returned to the pool after use or memory leaks will occur.
//============================================================================

#include "NetworkManager.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <batterytech/Logger.h>
#include "GameConnection.h"
#include "NetworkMessage.h"
#include "NetworkMessageListener.h"
#include "NetworkStatusListener.h"
#include <batterytech/Context.h>
#include "NetworkMessageFactory.h"
#include <string.h>

#define TCPDATAPORT "3490"  // the port users will be connecting to
#define HOSTUDPPORT "3490" // the port we use to find hosts and clients
#define CLIENTUDPPORT "3491" // the port we use to find hosts and clients
#define BACKLOG 10     // how many pending connections queue will hold
#define POOL_SIZE 20
#define QUEUE_SIZE 20
#define UDPBUFLEN 20
#define BROADCAST_INTERVAL 1.0f
#define DISCOVER_MESSAGE "BTMP"

namespace BatteryTech {

	NetworkManager::NetworkManager(Context *context) {
		this->context = context;
		listenerSocket = 0;
		udpListenerSocket = 0;
		_platform_init_network();
		connection = NULL;
		// objects will be created on-demand and pooled
		messagePool = new ManagedArray<NetworkMessage>(POOL_SIZE);
		outgoingQueue = new ManagedArray<NetworkMessage>(QUEUE_SIZE);
		messageListener = NULL;
		statusListener = NULL;
		status = NetworkStatusListener::STATUS_NONE;
		lastStatus = NetworkStatusListener::STATUS_NONE;
		broadcastTimeLeft = 0;
		netAddresses = NULL;
		netAddressCount = 0;
		currentNetAddress = 0;
		currentHostNumber = 0;
		udpSpammerSocket = 0;
		messageFactory = NULL;
	}

	NetworkManager::~NetworkManager() {
		if (connection) {
			connection->closeConnection();
		}
		delete connection;
		connection = NULL;
		_platform_release_network();
		messageListener = NULL;
		if (messagePool) {
			messagePool->deleteElements();
			delete messagePool;
		}
		messagePool = NULL;
		if (outgoingQueue) {
			outgoingQueue->deleteElements();
			delete outgoingQueue;
		}
		outgoingQueue = NULL;
		messageListener = NULL;
		statusListener = NULL;
		if (netAddresses) {
			delete netAddresses;
		}
		netAddresses = NULL;
		messageFactory = NULL;
	}

	void NetworkManager::setMessageFactory(NetworkMessageFactory *factory) {
		this->messageFactory = factory;
	}

	void NetworkManager::startHosting() {
		logmsg("Starting Hosting");
		// set up host TCP Listener
		struct addrinfo hints, *res;
		// load up address structs with getaddrinfo():
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE; // fill in my IP for me
		getaddrinfo("0.0.0.0", TCPDATAPORT, &hints, &res);
		// make a socket, bind it, and listen on it:
		listenerSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		// we use non-blocking sockets and a polling model
		_platform_make_non_blocking(listenerSocket);
		// silly APIs
		int yes = 1;
		// set it so we can reuse the socket even if it's already in use (by a previous process of ours, no doubt)
		if (setsockopt(listenerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int)) == -1) {
			logmsg("Error setting socket reuse address option");
		}
		if (bind(listenerSocket, res->ai_addr, res->ai_addrlen) < 0) {
			logmsg("Error binding to socket");
			status = NetworkStatusListener::STATUS_ERROR;
		} else {
			listen(listenerSocket, BACKLOG);
			logmsg("Listening for connections on 0.0.0.0:3490");
			status = NetworkStatusListener::STATUS_WAITING_FOR_CLIENTS;
		}
		startUDPListener(HOSTUDPPORT);
		// start spammer socket
		// load up address structs with getaddrinfo():
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE; // fill in my IP for me
		getaddrinfo("0.0.0.0", CLIENTUDPPORT, &hints, &res);
		// make a socket for udp spamming.
		udpSpammerSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	}

	void NetworkManager::findHost() {
		logmsg("Finding Host");
		status = NetworkStatusListener::STATUS_FINDING_HOSTS;
		startUDPListener(CLIENTUDPPORT);
		// Start spamming IPs
		if (!netAddresses) {
			// enumerate all IPs to send to.
			S32 hostnameCount;
			char** ifaddrs = _platform_get_ifaddrs(&hostnameCount);
			netAddresses = new char*[hostnameCount];
			netAddressCount = hostnameCount;
			for (S32 i = 0; i < hostnameCount; i++) {
				netAddresses[i] = new char[80];
				strcpy(netAddresses[i], ifaddrs[i]);
				logmsg(netAddresses[i]);
			}
			_platform_free_ifaddrs(ifaddrs, hostnameCount);
		}
		currentHostNumber = 0;
		currentNetAddress = 0;
		// start spammer socket
		struct addrinfo hints, *res;
		// load up address structs with getaddrinfo():
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE; // fill in my IP for me
		getaddrinfo("0.0.0.0", CLIENTUDPPORT, &hints, &res);
		// make a socket for udp spamming.
		udpSpammerSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	}

	void NetworkManager::connectToHost(const char *hostname) {
		char buf[50];
		sprintf(buf, "Connecting to host %s", hostname);
		logmsg(buf);
		stopUDPListener();
		// const char *hostname = "192.168.0.130";
		struct addrinfo hints, *res;
		// load up address structs with getaddrinfo():
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE; // fill in my IP for me
		getaddrinfo(hostname, TCPDATAPORT, &hints, &res);
		SOCKET sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		_platform_make_non_blocking(sock);
		BOOL32 connectOk = TRUE;
		if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
			int e = _platform_get_socket_last_error();
			if (e < 0) {
				if (e != EAGAIN && e != EWOULDBLOCK && e != ENOTCONN) {
					status = NetworkStatusListener::STATUS_ERROR;
					connectOk = FALSE;
				}
			}
		}
		if (connectOk) {
			connection = new GameConnection(sock, GameConnection::MODE_CLIENT, this);
			status = NetworkStatusListener::STATUS_CONNECTING;
		}
	}

	void NetworkManager::cancelAndClose() {
		stopUDPListener();
		// stop listening
		if (listenerSocket) {
			closesocket(listenerSocket);
		}
		listenerSocket = 0;
		if (udpSpammerSocket) {
			closesocket(udpSpammerSocket);
		}
		udpSpammerSocket = 0;
		// stop hosting
		// stop clients
		if (connection) {
			connection->closeConnection();
			delete connection;
		}
		connection = NULL;
		status = NetworkStatusListener::STATUS_NONE;
	}

	void NetworkManager::update() {
		if (status != lastStatus) {
			lastStatus = status;
			if (statusListener) {
				statusListener->onStatusChanged(status);
				if (status == NetworkStatusListener::STATUS_NONE || status == NetworkStatusListener::STATUS_ERROR) {
					messageListener->onNetworkDisconnected();
				}
			}
		}
		if (status == NetworkStatusListener::STATUS_WAITING_FOR_CLIENTS) {
			broadcastTimeLeft -= context->tickDelta;
			if (broadcastTimeLeft <= 0) {
				broadcastTimeLeft = BROADCAST_INTERVAL;
				// send UDP broadcast to clients via multicast/network
				sendUDPBCastMessage(CLIENTUDPPORT, DISCOVER_MESSAGE);
			}
		} else if (status == NetworkStatusListener::STATUS_FINDING_HOSTS) {
			// spam next IP / next network
			if (strcmp(netAddresses[currentNetAddress], "127.0.0.1") == 0) {
				currentNetAddress++;
				if (currentNetAddress == netAddressCount) {
					currentNetAddress = 0;
				}
			}
			currentHostNumber++;
			//currentHostNumber = 1;
			if (currentHostNumber == 255) {
				currentHostNumber = 1;
				currentNetAddress++;
				if (currentNetAddress == netAddressCount) {
					currentNetAddress = 0;
				}
			}
			const char *localIP4 = netAddresses[currentNetAddress];
			int a,b,c;
			sscanf(localIP4, "%d.%d.%d.", &a, &b, &c);
			char targetIP[16];
			sprintf(targetIP, "%d.%d.%d.%d", a, b, c, currentHostNumber);
			sendUDPMessage(targetIP, HOSTUDPPORT, DISCOVER_MESSAGE);
			//logmsg("Spamming:");
			//logmsg(targetIP);
		}
		if (udpListenerSocket) {
			//logmsg("updating udpListenerSocket");
			struct sockaddr_storage their_addr;
			socklen_t addr_len = sizeof their_addr;
			char buf[UDPBUFLEN];
			S32 numbytes;
			if ((numbytes = recvfrom(udpListenerSocket, buf, UDPBUFLEN-1 , 0,
				(struct sockaddr *)&their_addr, &addr_len)) == -1) {
				int e = _platform_get_socket_last_error();
				if (e < 0) {
					if (e != EAGAIN && e != EWOULDBLOCK && e != ENOTCONN) {
						logmsg("Recvfrom error");
					}
				}
			} else {
				logmsg("Got UDP packet");
				char s[INET6_ADDRSTRLEN];
				const char *hostname = inet_ntop(their_addr.ss_family,
						get_in_addr((struct sockaddr *)&their_addr),
						s, sizeof s);
				if (numbytes == strlen(DISCOVER_MESSAGE)) {
					buf[numbytes] = '\0';
					if (strcmp(DISCOVER_MESSAGE, buf) == 0) {
						logmsg("Packet contains discovery message");
						// it's us
						if (status == NetworkStatusListener::STATUS_WAITING_FOR_CLIENTS) {
							// host
							// tell the client we're here directly
							sendUDPMessage(hostname, CLIENTUDPPORT, DISCOVER_MESSAGE);
						} else if (status == NetworkStatusListener::STATUS_FINDING_HOSTS) {
							// client
							connectToHost(hostname);
						}
					}
				}
			}
		}
		if (listenerSocket) {
			//logmsg("updating listenerSocket");
			struct sockaddr_storage their_addr;
			socklen_t addr_size;
			SOCKET new_fd;
			addr_size = sizeof their_addr;
			new_fd = accept(listenerSocket, (struct sockaddr *) &their_addr, &addr_size);
			if (new_fd != (SOCKET)-1) {
				logmsg("Accepting new connection");
				_platform_make_non_blocking(new_fd);
				// create a client connection object and update it.
				connection = new GameConnection(new_fd, GameConnection::MODE_HOST, this);
				logmsg("Closing listener socket");
				closesocket(listenerSocket);
				listenerSocket = 0;
				stopUDPListener();
			}
		}
		if (connection) {
			//logmsg("updating connection");
			if (connection->state == GameConnection::CLOSED) {
				delete connection;
				connection = NULL;
			} else {
				connection->update();
			}
			if (outgoingQueue->getSize() > 0) {
				// send messages out in queue
				connection->sendMessages(outgoingQueue);
			}
		}
		// return queued messages to pool
		if (outgoingQueue->getSize() > 0) {
			//logmsg("returning queued messages to pool");
			for (S32 i = 0; i < outgoingQueue->getSize(); i++) {
				messagePool->add(outgoingQueue->array[i]);
			}
			outgoingQueue->clear();
		}
	}

	void NetworkManager::sendMessage(NetworkMessage *message) {
		// queue messages to be sent until the update, when they are batched and sent
		outgoingQueue->add(message);
		// update will put message back in pool when done
	}

	NetworkMessage* NetworkManager::createMessage(S32 messageType) {
		// attempt to pull from pool.  if pool is empty of this message type, create a new one.
		NetworkMessage *message = NULL;
		for (S32 i = 0; i < messagePool->getSize(); i++) {
			NetworkMessage *m = messagePool->array[i];
			if (m->messageType == messageType) {
				message = m;
				messagePool->remove(m);
				break;
			}
		}
		if (message) {
			return message;
		} else {
			if (!messageFactory) {
				logmsg("No Network Message Factory set!!");
				return NULL;
			} else {
				// message will be returned to pool after use, do not add it to pool here
				return messageFactory->createMessage(messageType);
			}
		}
		return NULL;
	}

	void NetworkManager::setMessageListener(NetworkMessageListener* listener) {
		this->messageListener = listener;
	}

	void NetworkManager::setStatusListener(NetworkStatusListener* listener) {
		this->statusListener = listener;
	}


	void NetworkManager::dispatchMessageToListener(NetworkMessage *message) {
		if (messageListener) {
			messageListener->onNetworkMessage(message);
		}
		returnToPool(message);
	}

	void NetworkManager::returnToPool(NetworkMessage *message) {
		messagePool->add(message);
	}

	void NetworkManager::startUDPListener(const char *port) {
		logmsg("Starting UDP listener");
		// set up host TCP Listener
		struct addrinfo hints, *res;
		// load up address structs with getaddrinfo():
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
		hints.ai_socktype = SOCK_DGRAM; // UDP
		hints.ai_flags = AI_PASSIVE; // fill in my IP for me
		getaddrinfo("0.0.0.0", port, &hints, &res); // listen on all interfaces
		// make a socket, bind it, and listen on it:
		udpListenerSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		// we use non-blocking sockets and a polling model
		_platform_make_non_blocking(udpListenerSocket);
		// silly APIs
		int yes = 1;
		// set it so we can reuse the socket even if it's already in use (by a previous process of ours, no doubt)
		if (setsockopt(udpListenerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int)) == -1) {
			logmsg("Error setting socket reuse address option");
		}
		if (bind(udpListenerSocket, res->ai_addr, res->ai_addrlen) < 0) {
			logmsg("Error binding to udp socket");
			status = NetworkStatusListener::STATUS_ERROR;
		} else {
			logmsg("Listening for udp clients...");
		}

	}

	void NetworkManager::stopUDPListener() {
		if (udpListenerSocket) {
			closesocket(udpListenerSocket);
		}
		udpListenerSocket = 0;
	}

	void NetworkManager::sendUDPMessage(const char *hostname, const char *port, const char *message) {
		char buf[50];
		sprintf(buf, "Sending UDP Message to %s", hostname);
		logmsg(buf);
		struct addrinfo hints, *res;
		// load up address structs with getaddrinfo():
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
		hints.ai_socktype = SOCK_DGRAM; // UDP
		hints.ai_flags = AI_PASSIVE; // fill in my IP for me
		getaddrinfo(hostname, port, &hints, &res);
		/*
		SOCKET sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		_platform_make_non_blocking(sock);
		*/
		int numbytes;
		// just reuse the udpListenerSocket to send.
		if (udpSpammerSocket == (SOCKET)-1) {
			logmsg("Error creating outbound UDP socket");
		} else {
			if ((numbytes = sendto(udpSpammerSocket, message, strlen(message), 0,
						 res->ai_addr, res->ai_addrlen)) == -1) {
				logmsg("Error sending UDP Message");
				checkErrors();
			}
		}
		freeaddrinfo(res);
	}

	void NetworkManager::sendUDPBCastMessage(const char *port, const char *message) {
		logmsg("Sending UDP Broadcast Message");
		struct addrinfo hints, *res;
		int yes = 1;
		// load up address structs with getaddrinfo():
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
		hints.ai_socktype = SOCK_DGRAM; // UDP
		hints.ai_flags = AI_PASSIVE; // fill in my IP for me
		getaddrinfo("255.255.255.255", port, &hints, &res);
		// make a socket, bind it, and listen on it:
		SOCKET sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		// should we be using a member broadcast socket for this so we're not always making a new one?
		int numbytes;
		if (sock == (SOCKET)-1) {
			logmsg("Error creating outbound UDP socket");
		} else {
			if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&yes,
					sizeof yes) == -1) {
				logmsg("Error setting broadcast flag");
			}
			if ((numbytes = sendto(sock, message, strlen(message), 0,
						 res->ai_addr, res->ai_addrlen)) == -1) {
				logmsg("Error sending UDP Message");
				checkErrors();
			}
			closesocket(sock);
		}
		freeaddrinfo(res);
	}

	void NetworkManager::checkErrors() {
		int e = _platform_get_socket_last_error();
		if (e != EAGAIN && e != EWOULDBLOCK) {
			// a real error
			if (e == EBADF) {
				logmsg("Bad file descriptor");
			} else if (e == ECONNRESET) {
				logmsg("Connection reset by peer");
			} else if (e == EINTR) {
				logmsg("Interrupted by signal");
			} else if (e == EINVAL) {
				logmsg("No out of band data available");
			} else if (e == ENOTCONN) {
				//normal while connecting on client
				logmsg("Socket not connected");
			} else if (e == ENOTSOCK) {
				logmsg("Not a socket!");
			} else if (e == EOPNOTSUPP) {
				logmsg("Specified flags not supported");
			} else if (e == ETIMEDOUT) {
				logmsg("Timed out");
			} else if (e == EIO) {
				logmsg("IO Error");
			} else if (e == ENOBUFS) {
				logmsg("Insufficient Resources");
			} else if (e == ENOMEM) {
				logmsg("Out of memory");
			} else {
				char buf[50];
				sprintf(buf, "Error no %d", e);
				logmsg(buf);
			}
		}
	}

}
