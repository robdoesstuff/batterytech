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
#include <batterytech/Context.h>
#include <batterytech/render/GraphicsConfiguration.h>
#include <batterytech/primitives.h>
#include "../World.h"
#include "B2DebugRenderer.h"
#include "SimpleSpriteRenderer.h"
#include "BallRenderer.h"

class B2DebugRenderer;
class SimpleSpriteRenderer;
class BallRenderer;

namespace BatteryTech { class TextRasterRenderer; }

using namespace BatteryTech;

class WorldRenderer: public Renderer {
public:
	WorldRenderer(Context *context);
	virtual void init(BOOL32 newContext);
	virtual void render(World *world);
	virtual ~WorldRenderer();
private:
	void renderDebugLabels(World *world);
	TextRasterRenderer *textRenderer;
	B2DebugRenderer *b2DebugRenderer;
	SimpleSpriteRenderer *farBgRenderer;
	SimpleSpriteRenderer *bgRenderer;
	SimpleSpriteRenderer *fgRenderer;
	SimpleSpriteRenderer *uiBGRenderer;
	BallRenderer *ballRenderer;
	GraphicsConfiguration *gConfig;
	Context *context;
	S32 frameSamplesCollected;
	F32 frameSampleTimeTotal;
	S32 fps;
};

#endif /* WORLDRENDERER_H_ */
