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
// Name        : NetworkMessageFactory.cpp
// Description : Implement in your application and set to the NetworkManager to receive network status updates.
//============================================================================

#ifndef NETWORKSTATUSLISTENER_H_
#define NETWORKSTATUSLISTENER_H_

namespace BatteryTech {

	/** \brief Implement this to listen for network status changes
	 * \ingroup Networking
	 * \class NetworkStatusListener
	 *
	 * Use in conjunction with NetworkManager
	 * \see NetworkManager
	 */
	class NetworkStatusListener {
	public:
		enum Status { STATUS_NONE, STATUS_ERROR, STATUS_WAITING_FOR_CLIENTS, STATUS_FINDING_HOSTS, STATUS_VALIDATING, STATUS_CONNECTING, STATUS_CONNECTED };
		virtual ~NetworkStatusListener(){};
		/** \brief Called when the network status has changes
		 * \param status The new network status
		 */
		virtual void onStatusChanged(NetworkStatusListener::Status status){};
	};

}
#endif /* NETWORKSTATUSLISTENER_H_ */
