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
// Name        : VibrationManager.cpp
// Description : Hardware vibrator control facility
//============================================================================

#include "VibrationManager.h"
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/Context.h>

namespace BatteryTech {

	VibrationManager::VibrationManager(Context *context) {
		this->context = context;
		enabled = TRUE;
	}

	VibrationManager::~VibrationManager() {
		context = NULL;
	}

	void VibrationManager::setEnabled(BOOL32 enabled) {
		this->enabled = enabled;
	}

	BOOL32 VibrationManager::isEnabled() {
		return enabled;
	}

	void VibrationManager::playEffect(S32 effectId, F32 intensity) {
		if (enabled) {
			_platform_play_vibration_effect(effectId, intensity);
		}
	}

	void VibrationManager::startEffect(S32 effectId, F32 intensity) {
		if (enabled) {
			_platform_start_vibration_effect(effectId, intensity);
		}
	}

	void VibrationManager::stopEffect(S32 effectId) {
		if (enabled) {
			_platform_stop_vibration_effect(effectId);
		}
	}

	void VibrationManager::stopAllEffects() {
		if (enabled) {
			_platform_stop_all_vibration_effects();
		}
	}

}
