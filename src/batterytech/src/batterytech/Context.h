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
// Name        : Context.h
// Description : A central point of access for input and all service-level objects including rendering, sound, networking and vibration
// Usage       : Replace Game and World with your own implementations to hook into the platform
//============================================================================

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "primitives.h"

class Game;
class UIManager;
class MenuRenderer;
class WorldRenderer;
class AudioManager;
class NetworkManager;
class VibrationManager;
class GraphicsConfiguration;
class World;
class RenderContext;

class Context {
public:
	struct PointerState {
		BOOL32 isDown;
		S32 x;
		S32 y;
	};
	struct AccelerometerState {
		F32 x;
		F32 y;
		F32 z;
	};
	Context(GraphicsConfiguration *gConfig);
	virtual ~Context();
	PointerState *pointerState;
	AccelerometerState accelerometerState;
	U8 keyPressed;
	BOOL32 isUIConsumingTouch;
	F32 tickDelta;
	BOOL32 showFPS;
	BOOL32 wasSuspended;

	Game *game;
	World *world;
	AudioManager *audioManager;
	NetworkManager *networkManager;
	VibrationManager *vibrationManager;
	WorldRenderer *worldRenderer;
	MenuRenderer *menuRenderer;
	GraphicsConfiguration *gConfig;
	UIManager *uiManager;
	RenderContext *renderContext;
};

#endif /* CONTEXT_H_ */
