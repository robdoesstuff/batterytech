/*
 * NetworkMessageFactory.h
 *
 *  Created on: Dec 29, 2010
 *      Author: rgreen
 */

#ifndef NETWORKMESSAGEFACTORY_H_
#define NETWORKMESSAGEFACTORY_H_

#include <batterytech/primitives.h>

class NetworkMessage;

class NetworkMessageFactory {
public:
	virtual ~NetworkMessageFactory(){};
	virtual NetworkMessage* createMessage(S32 messageType)=0;
};

#endif /* NETWORKMESSAGEFACTORY_H_ */
