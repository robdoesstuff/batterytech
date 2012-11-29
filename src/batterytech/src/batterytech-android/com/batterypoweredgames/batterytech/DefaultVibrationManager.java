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
// Name        : VibrationManager.java
// Description : Generic Vibration Manager.
// Usage       : Implement your own vibration patterns for Android here.
//============================================================================
package com.batterypoweredgames.batterytech;

import android.content.Context;
import android.os.Vibrator;

/**
 * Generic Vibration Manager.
 * 
 * Implement your own vibration patterns for Android here.
 */
public class DefaultVibrationManager implements VibrationManager {
	private Context context;
	private Vibrator vibe;
	
	public DefaultVibrationManager(Context context) {
		this.context = context;
		vibe = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
	}
	
	public void playVibrationEffect(int effectId, float intensity) {
		if (effectId == 0) {
			vibe.vibrate(50);
		}
	}

	public void startVibrationEffect(int effectId, float intensity) {
	}

	public void stopVibrationEffect(int effectId) {
	}

	public void stopAllVibrationEffects() {
	}

	public void release() {
		vibe = null;
		context = null;
	}
}
