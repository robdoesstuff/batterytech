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
// Name        : NetworkMessageListener.h
// Description : Implement in your application and set to the NetworkManager to receive network messages
//============================================================================

#ifndef NETWORKMESSAGELISTENER_H_
#define NETWORKMESSAGELISTENER_H_

namespace BatteryTech {

	class NetworkMessage;

	class NetworkMessageListener {
	public:
		virtual ~NetworkMessageListener(){};
		// called when a new network message has arrived.
		virtual void onNetworkMessage(NetworkMessage* message){};
		// called when the network connection has disconnected.
		virtual void onNetworkDisconnected(){};
	};

}
#endif /* NETWORKMESSAGELISTENER_H_ */
