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
// Name        : NetworkMessage.h
// Description : Subclass to make different types of network messages for your application.
//============================================================================

#ifndef NETWORKMESSAGE_H_
#define NETWORKMESSAGE_H_

#include <batterytech/primitives.h>

namespace BatteryTech {

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

}
#endif /* NETWORKMESSAGE_H_ */
