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
// Name        : batterytech_entrypoints.h
// Description : Defines a basic C++ interface for application update and rendering
//============================================================================

#ifndef BATTERYTECH_ENTRYPOINTS_H_
#define BATTERYTECH_ENTRYPOINTS_H_

class BTApplicationRenderer {
public:
	virtual void render()=0;
};

class BTApplicationUpdater {
public:
	virtual void update()=0;
};

#endif
