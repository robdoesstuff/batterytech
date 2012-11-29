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
// Name        : BatteryTechHookHandler.java
// Description : An interface to allow for customized hooks on an Android build
//============================================================================

package com.batterypoweredgames.batterytech;

public interface BatteryTechHookHandler {

	public void showAd();

	public void hideAd();
	
	public String hook(String hook);
}
