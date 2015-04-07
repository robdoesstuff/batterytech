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
// Name        : NetworkMessage.h
// Description : Subclass to make different types of network messages for your application.
//============================================================================

#ifndef NETWORKMESSAGE_H_
#define NETWORKMESSAGE_H_

#include <batterytech/primitives.h>

namespace BatteryTech {

	/** \brief A Network Message
	 * \ingroup Networking
	 * \class NetworkMessage
	 *
	 *
	 * This class encapsulates network messages to be used with NetworkManager.  It is abstract so you will need to subclass it with specific message type implementations.
	 * \see NetworkManager
	 */
	class NetworkMessage {
	public:
		/** \brief Creates a new network message
		 *  \param messageType The type of this message (user-defined)
		 */
		NetworkMessage(S32 messageType);
		virtual ~NetworkMessage();
		/** \brief encodes this message into a char buffer
		 *
		 * returns the number of chars added (payload length)
		 * \param buf The buffer to encode into
		 * \return The number of bytes encoded
		 */
		virtual S32 encode(char *buf)=0;
		/** \brief decodes this message from a char buffer
		 *
		 * \param buf The buffer to decode from
		 * \param length The length of the buffer
		 */
		virtual void decode(const char *buf, S32 length)=0;
		/** \brief the message type (user-defined) */
		S32 messageType;
		/** \brief the number of bytes of data in this message */
		S32 payloadLength;
	};

}
#endif /* NETWORKMESSAGE_H_ */
