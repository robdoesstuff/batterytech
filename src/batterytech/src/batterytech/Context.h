/*
 * Context.h
 *
 *  Created on: Oct 8, 2010
 *      Author: rgreen
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "primitives.h"

// cross-references
class Game;
class UIManager;
class MenuRenderer;
class WorldRenderer;
class AudioManager;
class NetworkManager;
class VibrationManager;
class GraphicsConfiguration;
class World;

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
};

#endif /* CONTEXT_H_ */
