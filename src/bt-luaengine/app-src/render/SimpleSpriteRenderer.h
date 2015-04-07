/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
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
