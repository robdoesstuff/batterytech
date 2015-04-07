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
// Name        : GameConnection.h
// Description : A TCP connection from a client to a NetworkManager server
// Usage       : Do not use directly.  Use NetworkManager to send messages to clients.
//============================================================================

#ifndef GAMECONNECTION_H_
#define GAMECONNECTION_H_

#include <batterytech/primitives.h>
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/util/ManagedArray.h>
#include "NetworkMessage.h"

namespace BatteryTech {

	class NetworkManager;

	/** \brief Manages an active game connection in either host or client mode
	 *
	 * Once the network manager has made a connection, it will create a GameConnection to the other side.  This GameConnection is what is used to transmit and receive
	 * data from a specific client or to the server.  Normally you don't need to do anything with this class because NetworkManager will use it internally to marshall
	 * data and manage the connection.
	 *
	 * \see NetworkManager
	 */
	class GameConnection {
	public:
		enum State { CONNECTING, WAITING_FOR_VALIDATION, VALIDATED, CLOSED };
		enum Mode { MODE_HOST, MODE_CLIENT };
		/** \brief Creates a new GameConnection
		 * \param socket The network socket
		 * \param mode The Mode
		 * \param netMgr The NetworkManager managing this connection
		 */
		GameConnection(SOCKET socket, Mode mode, NetworkManager *netMgr);
		virtual ~GameConnection();
		/** \brief Closes the connection
		 *
		 */
		virtual void closeConnection();
		/** \brief Updates the connection
		 *
		 */
		virtual void update();
		/** \brief Checks for errors
		 *
		 */
		void checkErrors();
		/** \brief Sends messages through the connection
		 *
		 * \param messages The list of messages to send
		 */
		void sendMessages(ManagedArray<NetworkMessage> *messages);
		SOCKET sock;
		State state;
		Mode mode;
	private:
		void bufferData();
		void pullFromDataBuffer(char *buf, S32 length);
		// incoming data buffer
		char *dataBuf;
		// incoming data buffer length
		S32 dataLength;
		NetworkManager *netMgr;
	};
}

#endif /* GAMECONNECTION_H_ */
