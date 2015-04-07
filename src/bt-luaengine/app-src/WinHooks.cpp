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


#ifdef _WIN32

#include <batterytech/primitives.h>
#include <batterytech/util/strx.h>
#include <batterytech/batterytech.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <windows.h>

using namespace std;

// This file is required for a Windows BatteryTech build.
// It implements several application-specific, platform-dependent functions.

DWORD CallbackThreadStart (LPVOID lpdwThreadParam );

void doCallback(const char *str) {
	char callback[512];
	sprintf(callback, "purchaseSucceeded %s", str);
	while (!btCallback(callback)) {
		Sleep(100);
	}
}

DWORD CallbackThreadStart (LPVOID lpdwThreadParam ) {
	doCallback((const char*) lpdwThreadParam);
	return 0;
}


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
		// if queue is full this won't work
		if (!btCallback(callback)) {
			cout << "Callback queue is full" << endl;
		}
	} else if (strStartsWith(hook, "restorePurchases")) {
		DWORD dwThreadId;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CallbackThreadStart, (LPVOID)"testproduct1", 0, &dwThreadId);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CallbackThreadStart, (LPVOID)"testproduct2", 0, &dwThreadId);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CallbackThreadStart, (LPVOID)"testproduct3", 0, &dwThreadId);
	} else if (strStartsWith(hook, "openURL")) {
		char hookData[512];
		strcpy(hookData, hook);
		strtok(hookData, " ");
		char *url = strtok(NULL, " ");
		ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
	} else if (strStartsWith(hook, "supportsOfferwall")) {
		strcpy(result, "true");
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
