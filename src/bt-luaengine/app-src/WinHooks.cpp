#ifdef _WIN32

#include <batterytech/primitives.h>
#include <batterytech/util/strx.h>
#include <batterytech/batterytech.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

// This file is required for a Windows BatteryTech build.
// It implements several application-specific, platform-dependent functions.

void winHook(const char *hook, char *result, S32 resultLen) {
	// Handle custom hooks here
	if (strStartsWith(hook, "requestPurchase")) {
		// call back with success
		char hookData[512];
		strcpy(hookData, hook);
		strtok(hookData, " ");
		char *productId = strtok(NULL, " ");
		char callback[512];
		sprintf(callback, "purchaseSucceeded %s", productId);
		btCallback(callback);
		cout << callback << endl;
	}
}

void winShowAd() {}

void winHideAd() {}

void winPlayVibrationEffect(S32 effectId, F32 intensity) {
	//cout << "Playing vibration effect " << effectId << " at " << intensity << endl;
}

void winStartVibrationEffect(S32 effectId, F32 intensity) {
	//cout << "Starting vibration effect " << effectId << " at " << intensity << endl;
}

void winStopVibrationEffect(S32 effectId) {
	//cout << "Stopping vibration effect " << effectId << endl;
}

void winStopAllVibrationEffects() {
	//cout << "Stopping all vibration effects" << endl;
}

#endif
