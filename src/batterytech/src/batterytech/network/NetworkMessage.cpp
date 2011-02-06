/*
 * NetworkMessage.cpp
 *
 *  Created on: Dec 24, 2010
 *      Author: rgreen
 */

#include "NetworkMessage.h"

NetworkMessage::NetworkMessage(S32 messageType) {
	this->messageType = messageType;
}

NetworkMessage::~NetworkMessage() {
	// TODO Auto-generated destructor stub
}
