/*
 * VibrationManager.cpp
 *
 *  Created on: Dec 29, 2010
 *      Author: rgreen
 */

#include "VibrationManager.h"
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/Context.h>

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

