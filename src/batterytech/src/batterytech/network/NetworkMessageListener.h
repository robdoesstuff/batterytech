/*
 * NetworkMessageListener.h
 *
 *  Created on: Dec 24, 2010
 *      Author: rgreen
 */

#ifndef NETWORKMESSAGELISTENER_H_
#define NETWORKMESSAGELISTENER_H_

class NetworkMessage;

class NetworkMessageListener {
public:
	virtual ~NetworkMessageListener(){};
	// called when a new network message has arrived.
	virtual void onNetworkMessage(NetworkMessage* message){};
	// called when the network connection has disconnected.
	virtual void onNetworkDisconnected(){};
};

#endif /* NETWORKMESSAGELISTENER_H_ */
