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
// Name        : GLSettings.java
// Description : Basic Android OpenGL Settings
//============================================================================

package com.batterypoweredgames.batterytech;

public class GLSettings {
	public boolean supportGLES2() { return false; }
	public int getDepthBits() { return 16; }
	public int getStencilBits() { return 0; }
	public boolean useTrueColor() { return false; }
}
