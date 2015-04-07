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
// Name        : GameConnection.cpp
// Description : A TCP connection from a client to a NetworkManager server
// Usage       : Do not use directly.  Use NetworkManager to send messages to clients.
//============================================================================

#include "GameConnection.h"
#include <batterytech/Logger.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "NetworkManager.h"
#include <batterytech/util/ByteUtil.h>
#include "NetworkStatusListener.h"

#define GREETING "BTCLIENT"
#define BUFFER_SIZE 1024

namespace BatteryTech {

	GameConnection::GameConnection(SOCKET socket, Mode mode, NetworkManager *netMgr) {
		this->sock = socket;
		this->mode = mode;
		this->netMgr = netMgr;
		if (mode == MODE_CLIENT) {
			state = CONNECTING;
			logmsg("Connecting");
		} else {
			state = WAITING_FOR_VALIDATION;
			logmsg("Waiting for Validation");
		}
		dataBuf = new char[BUFFER_SIZE];
		dataLength = 0;
	}

	GameConnection::~GameConnection() {
		sock = 0;
		delete [] dataBuf;
		dataBuf = NULL;
		netMgr = NULL;
	}

	void GameConnection::closeConnection() {
		if (sock) {
			logmsg("Closing game connection");
			closesocket(sock);
			state = CLOSED;
		}
		sock = 0;
		netMgr->status = NetworkStatusListener::STATUS_NONE;
	}

	void GameConnection::update() {
		if (!sock) {
			return;
		}
		// read data
		bufferData();
		if (state == WAITING_FOR_VALIDATION) {
			if (mode == MODE_HOST) {
				// listen to client greeting
				S32 greetingLength = strlen(GREETING);
				if (dataLength >= greetingLength) {
					char buf[255];
					pullFromDataBuffer(buf, greetingLength);
					buf[greetingLength] = '\0';
					if (strcmp(buf, GREETING) == 0) {
						logmsg("Validated Connection");
						state = VALIDATED;
						netMgr->status = NetworkStatusListener::STATUS_CONNECTED;
					} else {
						closeConnection();
					}
				}
			} else {
				// send client greeting
				logmsg("Sending greeting");
				S32 totalBytesSent = 0;
				while (totalBytesSent < strlen(GREETING) && sock) {
					S32 bytesSent = send(sock, GREETING, strlen(GREETING), 0);
					if (bytesSent == 0) {
						closeConnection();
					} else if (bytesSent == -1) {
						checkErrors();
					} else if (bytesSent > 0) {
						totalBytesSent += bytesSent;
					}
				}
				// TODO probably should get a response from the host!
				state = VALIDATED;
				logmsg("Validated");
				netMgr->status = NetworkStatusListener::STATUS_CONNECTED;
			}
		} else {
			// S32 totalPayloadLength (including this S32 and messageCount S32)
			// S32 messageCount
			// --
			// S32 messagePayloadLength (not including this 8 byte header)
			// S32 messageType (not included in messagePayloadLength)
			// var messagePayload of length messagePayloadLength
			// ...
			// if bufferlength is equal or bigger than total payload length, read into scratch.
			// for each message, create type from factory and decode from scratch at position, then callback to listener
			// then return to pool.
			if (dataLength >= 4) {
				// read payload length and wait for rest
				S32 totalPayloadLength = ByteUtil::fromBytesS32(dataBuf, 0);
				if (dataLength >= totalPayloadLength) {
					// we have a complete packet
					char buf[BUFFER_SIZE];
					pullFromDataBuffer(buf, totalPayloadLength);
					S32 messageCount = ByteUtil::fromBytesS32(buf, 4);
					S32 bufPos = 8;
					for (S32 i = 0; i < messageCount; i++) {
						S32 messagePayloadLength = ByteUtil::fromBytesS32(buf, bufPos);
						S32 messageType = ByteUtil::fromBytesS32(buf, bufPos + 4);
						bufPos += 8;
						NetworkMessage *message = netMgr->createMessage(messageType);
						message->decode(buf + bufPos, messagePayloadLength);
						bufPos += messagePayloadLength;
						netMgr->dispatchMessageToListener(message);
					}
				}
			}
		}
	}

	void GameConnection::sendMessages(ManagedArray<NetworkMessage> *messages) {
		// S32 totalPayloadLength (including this S32)
		// S32 messageCount
		// --
		// S32 messagePayloadLength
		// S32 messageType
		// var messagePayload
		// count messages
		S32 messageCount = messages->getSize();
		S32 totalPayloadLength = 8;
		for (S32 i = 0; i < messageCount; i++) {
			totalPayloadLength += 8; // for 2 S32 message header
			totalPayloadLength += messages->array[i]->payloadLength;
		}
		char buf[BUFFER_SIZE];
		// 8 byte header
		ByteUtil::toBytes(buf, 0, totalPayloadLength);
		ByteUtil::toBytes(buf, 4, messageCount);
		S32 bufPos = 8;
		for (S32 i = 0; i < messageCount; i++) {
			NetworkMessage *message = messages->array[i];
			ByteUtil::toBytes(buf, bufPos, message->payloadLength);
			ByteUtil::toBytes(buf, bufPos + 4, message->messageType);
			bufPos += 8;
			message->encode(buf + bufPos);
			bufPos += message->payloadLength;
		}
		S32 totalBytesSent = 0;
		while (totalBytesSent < totalPayloadLength && sock) {
			S32 bytesSent = send(sock, buf + totalBytesSent, totalPayloadLength - totalBytesSent, 0);
			if (bytesSent == -1) {
				checkErrors();
			} else if (bytesSent == 0) {
				logmsg("Connection Lost");
				closeConnection();
			}
			if (bytesSent > 0) {
				totalBytesSent += bytesSent;
			}
		}
	}

	void GameConnection::bufferData() {
		char buf[BUFFER_SIZE];
		S32 count = recv(sock, buf, BUFFER_SIZE - dataLength, 0);
		if (count == -1) {
			checkErrors();
		} else if (count == 0) {
			logmsg("Connection Lost");
			closeConnection();
		} else {
			// append to dataBuf
			memcpy(dataBuf+(dataLength * sizeof(char)), buf, count);
			dataLength += count;
		}
	}

	void GameConnection::pullFromDataBuffer(char *buf, S32 length) {
		memcpy(buf, dataBuf, length);
		if (dataLength - length > 0) {
			S32 remainder = dataLength - length;
			// move bytes in data buffer back to 0
			char tempBuf[BUFFER_SIZE];
			memcpy(tempBuf, dataBuf+length, remainder);
			memcpy(dataBuf, tempBuf, remainder);
			dataLength = remainder;
		} else {
			dataLength = 0;
		}
	}

	void GameConnection::checkErrors() {
		int e = _platform_get_socket_last_error();
		if (e != EAGAIN && e != EWOULDBLOCK) {
			// a real error
			if (e == EBADF) {
				logmsg("Bad file descriptor");
			} else if (e == ECONNRESET) {
				logmsg("Connection reset by peer");
				closeConnection();
			} else if (e == EINTR) {
				logmsg("Interrupted by signal");
			} else if (e == EINVAL) {
				logmsg("No out of band data available");
			} else if (e == ENOTCONN) {
				//normal while connecting on client
				if (state != CONNECTING) {
					logmsg("Socket not connected");
				}
			} else if (e == ENOTSOCK) {
				logmsg("Not a socket!");
				closeConnection();
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
		} else {
			if (state == CONNECTING) {
				// this is the non-blocking "Error" during a connection
				logmsg("Connected");
				state = WAITING_FOR_VALIDATION;
				netMgr->status = NetworkStatusListener::STATUS_VALIDATING;
			}
		}
	}
}
