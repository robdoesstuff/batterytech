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
// Name        : VibrationManager.h
// Description : Hardware vibrator control facility
//============================================================================

#ifndef VIBRATIONMANAGER_H_
#define VIBRATIONMANAGER_H_

#include <batterytech/primitives.h>

namespace BatteryTech {

	class Context;

	// Besides IOS, this won't do anything unless you implement the vibration effects on the platform side.
	// Only numeric IDs are passed to avoid allocations in JNI for Android.
	// See the demo app VibrationManager class for the Java hooks for Android to implement vibrations.
	class VibrationManager {
	public:
		VibrationManager(Context *context);
		virtual ~VibrationManager();
		/**
		 * Plays a vibration effect at a given intensity
		 * Dependent on platform implementation.
		 */
		virtual void playEffect(S32 effectId, F32 intensity);
		/**
		 * Starts a vibration effect at a given intensity (the idea is that it plays looping until stopped)
		 * Dependent on platform implementation.
		 */
		virtual void startEffect(S32 effectId, F32 intensity);
		/**
		 * Stops a looping vibration effect
		 * Dependent on platform implementation.
		 */
		virtual void stopEffect(S32 effectId);
		/**
		 * Stops all vibration effects
		 * Dependent on platform implementation.
		 */
		virtual void stopAllEffects();
		/**
		 * Enables/Disables vibration
		 */
		virtual void setEnabled(BOOL32 enabled);
		/**
		 * Is vibration enabled?
		 */
		virtual BOOL32 isEnabled();
	protected:
		Context *context;
		BOOL32 enabled;
	};

}
#endif /* VIBRATIONMANAGER_H_ */
