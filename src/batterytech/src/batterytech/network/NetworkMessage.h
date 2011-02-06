/*
 * NetworkMessage.h
 *
 *  Created on: Dec 24, 2010
 *      Author: rgreen
 */

#ifndef NETWORKMESSAGE_H_
#define NETWORKMESSAGE_H_

#include <batterytech/primitives.h>

class NetworkMessage {
public:
	NetworkMessage(S32 messageType);
	virtual ~NetworkMessage();
	// encodes this message into a char buffer
	// returns the number of chars added (payload length)
	virtual S32 encode(char *buf)=0;
	// decodes this message from a char buffer
	virtual void decode(const char *buf, S32 length)=0;
	S32 messageType;
	S32 payloadLength;
};

#endif /* NETWORKMESSAGE_H_ */
