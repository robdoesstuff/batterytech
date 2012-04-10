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
// Name        : Context.cpp
// Description : A central point of access for input and all service-level objects including rendering, sound, networking and vibration
// Usage       : Replace Game and World with your own implementations to hook into the platform
//============================================================================

#include "Context.h"
#include "render/MenuRenderer.h"
#include "audio/AudioManager.h"
#include "network/NetworkManager.h"
#include "VibrationManager.h"
#include "render/GraphicsConfiguration.h"
#include "ui/UIManager.h"
#include "render/RenderContext.h"
#include "batterytech_globals.h"
#include "render/GLResourceManager.h"

namespace BatteryTech {

	Context::Context(GraphicsConfiguration *gConfig) {
		this->gConfig = gConfig;
		glResourceManager = new GLResourceManager(this);
		menuRenderer = new MenuRenderer(this);
		audioManager = new AudioManager();
		networkManager = new NetworkManager(this);
		vibrationManager = new VibrationManager(this);
		uiManager = new UIManager(this);
		renderContext = new RenderContext();
		tickDelta = 0;
		isUIConsumingPointers = FALSE;
		isUIConsumingKeys = FALSE;
		keyPressed = 0;
		specialKeyPressed = SKEY_NULL;
		showFPS = FALSE;
		wasSuspended = FALSE;
		accelerometerState.x = 0;
		accelerometerState.y = 0;
		accelerometerState.z = 0;
		pointerState = new PointerState[MAX_POINTERS];
		for (S32 i = 0; i < MAX_POINTERS; i++) {
			pointerState[i].isDown = FALSE;
			pointerState[i].x = 0;
			pointerState[i].y = 0;
		}
		keyState = new KeyState[MAX_KEYSTATES];
		for (S32 i = 0; i < MAX_POINTERS; i++) {
			keyState[i].isDown = FALSE;
			keyState[i].keyCode = 0;
		}
		callbackData[0] = '\0';
		callbackDataReady = FALSE;
	}

	Context::~Context() {
		logmsg("Releasing Context");
		if (audioManager) {
			delete audioManager;
		}
		audioManager = NULL;
		delete networkManager;
		networkManager = NULL;
		delete vibrationManager;
		vibrationManager = NULL;
		delete glResourceManager;
		glResourceManager = NULL;
		delete uiManager;
		uiManager = NULL;
		delete renderContext;
		renderContext = NULL;
		delete [] pointerState;
		logmsg("Done Releasing Context");
	}

}
