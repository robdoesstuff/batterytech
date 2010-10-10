/*
 * Context.h
 *
 *  Created on: Oct 8, 2010
 *      Author: rgreen
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "game/Game.h"
#include "game/World.h"
#include "render/WorldRenderer.h"
#include "render/MenuRenderer.h"
#include "sound/SoundManager.h"
#include "render/GraphicsConfiguration.h"
#include "ui/UIManager.h"
#include "primitives.h"

// cross-references
class Game;
class UIManager;
class MenuRenderer;

class Context {
public:
	Context(GraphicsConfiguration *gConfig);
	virtual ~Context();
	F32 tickDelta;
	BOOL32 down1;
	BOOL32 down2;
	S32 x1,y1;
	S32 x2,y2;

	Game *game;
	World *world;
	SoundManager *soundManager;
	WorldRenderer *worldRenderer;
	MenuRenderer *menuRenderer;
	GraphicsConfiguration *gConfig;
	UIManager *uiManager;
};

#endif /* CONTEXT_H_ */
