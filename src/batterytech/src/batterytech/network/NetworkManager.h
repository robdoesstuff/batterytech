/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
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

#ifndef NETWORKMANAGER_H_
#define NETWORKMANAGER_H_

#include <batterytech/platform/platformgeneral.h>
#include <batterytech/util/ManagedArray.h>
#include "NetworkStatusListener.h"

namespace BatteryTech {

	class Context;
	class GameConnection;
	class NetworkMessage;
	class NetworkMessageListener;
	class NetworkStatusListener;
	class NetworkMessageFactory;

	class NetworkManager {
	public:
		NetworkManager(Context *context);
		virtual ~NetworkManager();
		// opens a socket and listens for clients
		void startHosting();
		// scans the network for the host socket and connects
		void findHost();
		// stops hosting/searching/client and closes any and all client connections
		void cancelAndClose();
		// polls network
		void update();
		// Queues messages to be sent over the network.
		void sendMessage(NetworkMessage *message);
		// Creates a message to be used to send over the network.
		// If you call this, you MUST send the message or there will be a memory leak.
		// This is a pooling mechanism which is also used internally to do message callbacks
		NetworkMessage* createMessage(S32 messageType);
		void setMessageListener(NetworkMessageListener *listener);
		void setStatusListener(NetworkStatusListener *listener);
		void dispatchMessageToListener(NetworkMessage *message);
		// know what you're doing when you call this!
		void returnToPool(NetworkMessage *message);
		void setMessageFactory(NetworkMessageFactory *factory);
		NetworkStatusListener::Status status;
		NetworkStatusListener::Status lastStatus;
	private:
		void startUDPListener(const char *port);
		void stopUDPListener();
		void connectToHost(const char *hostname);
		void sendUDPMessage(const char *hostname, const char *port, const char *message);
		void sendUDPBCastMessage(const char *port, const char *message);
		void checkErrors();
		NetworkMessageFactory *messageFactory;
		SOCKET udpListenerSocket;
		SOCKET listenerSocket;
		SOCKET udpSpammerSocket;
		GameConnection *connection;
		ManagedArray<NetworkMessage> *messagePool;
		ManagedArray<NetworkMessage> *outgoingQueue;
		NetworkMessageListener *messageListener;
		NetworkStatusListener *statusListener;
		Context *context;
		F32 broadcastTimeLeft;
		// these next 4 fields are for a spammy hack in which the client sends UDP to every host on all subnets its connected to.
		// the net addresses are the IPv4 addresses of the local IFs
		char **netAddresses;
		// how many net addresses there are
		S32 netAddressCount;
		// which net address we're currently spamming
		S32 currentNetAddress;
		// which host number (x.x.x.1 = 1, x.x.x.254 = 254);
		S32 currentHostNumber;
	};

}
#endif /* NETWORKMANAGER_H_ */
