/*
 * GameConnection.h
 *
 *  Created on: Dec 23, 2010
 *      Author: rgreen
 */

#ifndef GAMECONNECTION_H_
#define GAMECONNECTION_H_

#include <batterytech/primitives.h>
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/util/ManagedArray.h>
#include "NetworkMessage.h"

class NetworkManager;

class GameConnection {
public:
	enum State { CONNECTING, WAITING_FOR_VALIDATION, VALIDATED, CLOSED };
	enum Mode { MODE_HOST, MODE_CLIENT };
	GameConnection(SOCKET socket, Mode mode, NetworkManager *netMgr);
	virtual ~GameConnection();
	virtual void closeConnection();
	virtual void update();
	void checkErrors();
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

#endif /* GAMECONNECTION_H_ */
