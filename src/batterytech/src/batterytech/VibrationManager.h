/*
 * VibrationManager.h
 *
 *  Created on: Dec 29, 2010
 *      Author: rgreen
 */

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
