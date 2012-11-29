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
// Name        : TouchProcessor.java
// Description : An implementation of this interface will handle touch events and convert them into touch objects
//============================================================================

package com.batterypoweredgames.input;

import java.util.concurrent.ArrayBlockingQueue;

import com.batterypoweredgames.batterytech.Boot;

import android.view.MotionEvent;

public interface TouchProcessor {
	public void processTouchEvent(MotionEvent event, ArrayBlockingQueue<InputObject> inputObjectPool, InputHandler boot);
}
