#ifdef _WIN32

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
