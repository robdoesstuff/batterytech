/*
 * NetworkStatusListener.h
 *
 *  Created on: Dec 27, 2010
 *      Author: rgreen
 */

#ifndef NETWORKSTATUSLISTENER_H_
#define NETWORKSTATUSLISTENER_H_

class NetworkStatusListener {
public:
	enum Status { STATUS_NONE, STATUS_ERROR, STATUS_WAITING_FOR_CLIENTS, STATUS_FINDING_HOSTS, STATUS_VALIDATING, STATUS_CONNECTING, STATUS_CONNECTED };
	virtual ~NetworkStatusListener(){};
	virtual void onStatusChanged(NetworkStatusListener::Status status){};
};

#endif /* NETWORKSTATUSLISTENER_H_ */
