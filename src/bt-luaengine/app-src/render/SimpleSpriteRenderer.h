/*
 * SimpleSpriteRenderer.h
 *
 *  Created on: Dec 2, 2010
 *      Author: rgreen
 */

#ifndef SIMPLESPRITERENDERER_H_
#define SIMPLESPRITERENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/render/ShaderProgram.h>
#include "RenderItem.h"

using namespace BatteryTech;

class GameContext;

/** \brief A very basic sprite renderer
 *
 * Translates RenderItems into QuadRenderer calls
 *
 */
class SimpleSpriteRenderer : Renderer {
public:
	SimpleSpriteRenderer(GameContext *context);
	virtual void init(BOOL32 newContext);
	/** \brief Renders a sprite using RenderItem data
	 *
	 * This will use the texture and automatically apply any trim offsets or UV transforms from a texture atlas.
	 *
	 * \param item The item to render
	 */
	virtual void render(RenderItem *item);
    void startBatch();
    void endBatch();
	virtual ~SimpleSpriteRenderer();
private:
	GameContext *context;
};

#endif /* SIMPLESPRITERENDERER_H_ */
