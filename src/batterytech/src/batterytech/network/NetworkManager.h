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

	/** \brief Peer Discovery and Host/Client networking
	 * \ingroup Networking
	 * \class NetworkManager
	 *
	 *
	 * NetworkManager provides a host mode, client mode and some status and message listener hooks.
	 * The host starts hosting and when a client begins searching for a host, they will find each
	 * other and the data connection will start.
	 *
	 * Both Host and Client need to register listeners (normally your game or other class) using \ref setMessageListener and \ref setStatusListener\n
	 * Host calls:  \ref startHosting()\n
	 * Client calls: \ref findHost()\n
	 * and they will receive callbacks via the \ref NetworkStatusListener.

	 * To cancel, they can just call \ref cancelAndClose() and it will quit.  Also if either side quits or network errors occur, the status listener
	 * will be called with the new status.

	 * Ports and handshake messages can be all changed in NetworkManager.cpp

	 * Once connected, hosts/clients (we'll call them peers here on out) can send each other messages.  A message is a custom class that represents
	 * one or more pieces of data.

	 * Here is an example NetworkMessage:

	 * \code
	 * // put your different message type constants in your game constants file
	 * #define NETWORK_MESSAGE_TYPE_GAMESTATE 0
	 *
	 * // GameStateMessage.h
	 * class GamestateMessage : public NetworkMessage {
	 * public:
	 * 	GamestateMessage();
	 * 	virtual ~GamestateMessage();
	 * 	virtual S32 encode(char *buf);
	 * 	virtual void decode(const char *buf, S32 length);
	 * 	S32 level;
	 * 	U8 gameState;
	 * };
	 *
	 * // GameStateMessage.cpp
	 * GamestateMessage::GamestateMessage() : NetworkMessage(NETWORK_MESSAGE_TYPE_GAMESTATE) {
	 * 	level = 0;
	 * 	gameState = 0;
	 * 	payloadLength = 5;
	 * }
	 *
	 * GamestateMessage::~GamestateMessage() {
	 * }
	 *
	 * S32 GamestateMessage::encode(char *buf) {
	 * 	// encode into little endian
	 * 	ByteUtil::toBytes(buf, 0, level);
	 * 	buf[4] = gameState;
	 * 	return payloadLength;
	 * }
	 *
	 * void GamestateMessage::decode(const char *buf, S32 length) {
	 * 	// decode from little endian
	 * 	level = ByteUtil::fromBytesS32(buf, 0);
	 * 	gameState = buf[4];
	 * }
	 *
	 * And to create this message (and your other message types), you will need a NetworkMessageFactory:
	 *
	 * // MyMessageFactory.h
	 * class MyMessageFactory: public NetworkMessageFactory {
	 * public:
	 * 	MyMessageFactory: ();
	 * 	virtual MyMessageFactory: ();
	 * 	virtual NetworkMessage* createMessage(S32 messageType);
	 * };
	 *
	 * // MyMessageFactory.cpp
	 *
	 * MyMessageFactory::MyMessageFactory() {
	 * }
	 *
	 * MyMessageFactory::~MyMessageFactory() {
	 * }
	 *
	 * NetworkMessage* MyMessageFactory::createMessage(S32 messageType) {
	 * 	if (messageType == NETWORK_MESSAGE_TYPE_GAMESTATE) {
	 * 		return new GamestateMessage();
	 * 	} else {
	 * 		return NULL;
	 * 	}
	 * 	// add more message types here
	 * }
	 * \endcode
	 * So if you can see from the code, the MessageFactory instantiates types of messages given by the S32 messageType.  This is used by the NetworkManager to callback to your app with messages received from the network.  You simply set the factory by calling:
	 *
	 * \code
	 * MyMessageFactory *myMessageFactory = new MyMessageFactory();
	 * context->networkManager->setMessageFactory(myMessageFactory);
	 * \endcode
	 *
	 * and you will need to delete your message factory on app shutdown as NetworkManager will not do that for you.
	 *
	 * The class ByteUtil is provided to easily convert values to bytes and back.  Every 16-64 bit standard type is supported.
	 *
	 * MessageFactory only sends TCP data.  For more advanced networking, using UDP for high performance, it is recommended that you develop your own messaging engine as there are usually more than one type of message (some critical, some not) and that is beyond the scope of BatteryTech.
	 *
	 * 	 * A note on NetworkMessage object pooling
	 *
	 * NetworkMessages are pooled by the NetworkManager.  They will always be returned to the message pool automatically after sending or receiving.  Just remember:  _never_ instantiate your own messages and send them using this NetworkManager.  You will pollute the pool and cause all types of problems.  Always use the NetworkManager createMessage() function to create a message.  Messages will be returned to the pool automatically after being sent.  See the following:
	 *
	 * // this is the correct way to send a gamestate message to the client:
	 * \code
	 * GamestateMessage *msg = context->networkManager->createMessage(NETWORK_MESSAGE_TYPE_GAMESTATE);
	 * msg->level = someLevel;
	 * msg->gameState = GAMESTATE_GAME_OVER;
	 * context->networkManager->sendMessage(msg);
	 *
	 * // this is the correct way to handle receiving a message (assuming Game extends NetworkMessageListener)
	 *
	 * void Game::onNetworkMessage(NetworkMessage* message) {
	 * if (message->messageType == NETWORK_MESSAGE_TYPE_GAMESTATE) {
	 * 	GamestateMessage *gsMessage = (GamestateMessage*) message;
	 * 	getWorld()->level = gsMessage->level;
	 * 	getWorld()->gameState = gsMessage->gameState;
	 * }
	 * \endcode
	 */
	class NetworkManager {
	public:
		NetworkManager(Context *context);
		virtual ~NetworkManager();
		/** \brief opens a socket and listens for clients
		 *
		 */
		void startHosting();
		/** \brief scans the network for the host socket and connects
		 *
		 */
		void findHost();
		/** \brief stops hosting/searching/client and closes any and all client connections
		 *
		 */
		void cancelAndClose();
		/** \brief polls network
		 *
		 */
		void update();
		/** \brief Queues messages to be sent over the network.
		 *
		 *	\param message The message to send
		 */
		void sendMessage(NetworkMessage *message);
		/** \brief Creates a message to be used to send over the network.
		 *
		 * If you call this, you MUST send the message or there will be a memory leak.\n
		 * This is a pooling mechanism which is also used internally to do message callbacks
		 * \param messageType An application-specific message type (you define it yourself for your app)
		 */
		NetworkMessage* createMessage(S32 messageType);
		/** \brief Sets the message listener
		 *
		 * Incoming network messages will be sent to the listener
		 * \param listener The network message listener
		 */
		void setMessageListener(NetworkMessageListener *listener);
		/** \brief Sets the status listener
		 *
		 * Network status events will be sent to the listener
		 * \param listener The network status listener
		 */
		void setStatusListener(NetworkStatusListener *listener);
		/** \brief Dispatches a message to the message listener
		 *
		 * \param message The message to dispatch
		 */
		void dispatchMessageToListener(NetworkMessage *message);
		/** \brief Returns a network message to the message pool
		 *
		 * know what you're doing when you call this!  See class docs for more details
		 */
		void returnToPool(NetworkMessage *message);
		/** \brief Sets the factory that will create messages of user-defined types
		 *
		 * \param factory The message factory
		 */
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
