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
// Name        : batterytech_entrypoints.h
// Description : Defines a basic C++ interface for application update and rendering
//============================================================================

#ifndef BATTERYTECH_ENTRYPOINTS_H_
#define BATTERYTECH_ENTRYPOINTS_H_

/** \brief Entry point for rendering a BatteryTech application
 *
 * This class needs to be extended and added to an extended Context to provide rendering capabilities.
 * \ingroup Entry
 */
class BTApplicationRenderer {
public:
	/** \brief Renders the application
	 *
	 */
	virtual void render()=0;
};

/** \brief Entry point for updating a BatteryTech application
 *
 * In a normal state-driven application, update is where the processing logic should be.  It is a good practice to keep all rendering code separate, but
 * of course calculations needed to drive rendering later can be fine in phase.
 *
 * This class needs to be extended and added to an extended Context to provide update capabilities.
 * \ingroup Entry
 */
class BTApplicationUpdater {
public:
	/** \brief Updates the application
	 *
	 * Time delta in seconds from the last update is available via Context::tickDelta
	 */
	virtual void update()=0;
	/** \brief Informs the application that it is about to be suspended
	 *
	 * This function is called synchronously and the whole application context should still be intact so if you need to save before you die, save now.
	 */
	virtual void onSuspend() {};
	/** \brief Informs the application that it was just resumed
	 *
	 * While it's easy to determine because of the previous call to onSuspend and now an update is about to occur, it's still nice to get a callback for those who
	 * want to add special handling here.
	 */
	virtual void onResume() {};
};

#endif
