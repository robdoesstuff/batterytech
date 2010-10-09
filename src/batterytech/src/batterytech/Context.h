/*
 * Context.h
 *
 *  Created on: Oct 8, 2010
 *      Author: rgreen
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "game/World.h"
#include "render/WorldRenderer.h"
#include "render/MenuRenderer.h"
#include "sound/SoundManager.h"
#include "render/GraphicsConfiguration.h"
#include "ui/UIManager.h"

class Context {
public:
	Context(GraphicsConfiguration *gConfig);
	virtual ~Context();
	World *world;
	SoundManager *soundManager;
	WorldRenderer *worldRenderer;
	MenuRenderer *menuRenderer;
	GraphicsConfiguration *gConfig;
	UIManager *uiManager;
};

#endif /* CONTEXT_H_ */
