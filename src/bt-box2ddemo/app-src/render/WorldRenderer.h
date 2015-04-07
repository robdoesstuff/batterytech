/*
 * WorldRenderer.h
 *
 *  Created on: Sep 4, 2010
 *      Author: rgreen
 */

#ifndef WORLDRENDERER_H_
#define WORLDRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/render/TextRasterRenderer.h>
#include "../GameContext.h"
#include <batterytech/render/GraphicsConfiguration.h>
#include <batterytech/primitives.h>
#include "../World.h"
#include "B2DebugRenderer.h"
#include "BallRenderer.h"
#include <batterytech/batterytech_entrypoints.h>

class B2DebugRenderer;
class SimpleSpriteRenderer;
class BallRenderer;

namespace BatteryTech { class TextRasterRenderer; }

using namespace BatteryTech;

class WorldRenderer: public Renderer, public BTApplicationRenderer {
public:
	WorldRenderer(GameContext *context);
	virtual void init(BOOL32 newGameContext);
	// Implementing BTApplicationUpdater::render()
	virtual void render();
	virtual ~WorldRenderer();
private:
	void renderDebugLabels(World *world);
	TextRasterRenderer *textRenderer;
	B2DebugRenderer *b2DebugRenderer;
	BallRenderer *ballRenderer;
	GraphicsConfiguration *gConfig;
	GameContext *context;
	S32 frameSamplesCollected;
	F32 frameSampleTimeTotal;
	S32 fps;
};

#endif /* WORLDRENDERER_H_ */
