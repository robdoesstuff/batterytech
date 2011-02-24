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
// Name        : VibrationManager.h
// Description : Hardware vibrator control facility
//============================================================================

#ifndef VIBRATIONMANAGER_H_
#define VIBRATIONMANAGER_H_

#include <batterytech/primitives.h>

class Context;

class VibrationManager {
public:
	VibrationManager(Context *context);
	virtual ~VibrationManager();
	virtual void playEffect(S32 effectId, F32 intensity);
	virtual void startEffect(S32 effectId, F32 intensity);
	virtual void stopEffect(S32 effectId);
	virtual void stopAllEffects();
	virtual void setEnabled(BOOL32 enabled);
	virtual BOOL32 isEnabled();
protected:
	Context *context;
	BOOL32 enabled;
};

#endif /* VIBRATIONMANAGER_H_ */
