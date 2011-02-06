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
class SoundManager;
class NetworkManager;
class VibrationManager;
class GraphicsConfiguration;
class World;

class Context {
public:
	enum NetState { NONE, HOST, CLIENT };
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
	F32 tickDelta;
	BOOL32 down1;
	BOOL32 down2;
	S32 x1,y1;
	S32 x2,y2;
	BOOL32 isUIConsumingTouch;
	U8 keyPressed;
	BOOL32 showFPS;
	BOOL32 wasSuspended;

	Game *game;
	World *world;
	SoundManager *soundManager;
	NetworkManager *networkManager;
	VibrationManager *vibrationManager;
	WorldRenderer *worldRenderer;
	MenuRenderer *menuRenderer;
	GraphicsConfiguration *gConfig;
	UIManager *uiManager;
	NetState netState;
};

#endif /* CONTEXT_H_ */
