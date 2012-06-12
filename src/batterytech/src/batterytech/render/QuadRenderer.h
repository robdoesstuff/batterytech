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
// Name        : QuadRenderer.h
// Description : Renders Quads
//============================================================================

#ifndef QUADRENDERER_H_
#define QUADRENDERER_H_

#include "Renderer.h"
#include "../primitives.h"
#include "../platform/platformgl.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

using namespace BatteryTech;

namespace BatteryTech {

class Context;

/** \brief A common quad (2 triangles as a rectangle AKA Sprite) renderer
 *
 * Renders one quad at a time.
 *
 */
class QuadRenderer : Renderer {
public:
	QuadRenderer(Context *context);
	virtual void init(BOOL32 newContext);
	/** \brief Basic render
	 *
	 * Does not assume any texture transform or offsets
	 *
	 */
	void render(Texture *texture, F32 top, F32 right, F32 bottom, F32 left);
	/** \brief Basic render
	 *
	 * Does not assume any texture transform or offsets
	 *
	 */
	void render(Texture *texture, F32 x, F32 y, F32 width, F32 height, F32 angle);

	void render(Texture *texture, Vector3f pos, F32 angleRads, Vector4f myUvs, Vector2f scale, F32 alpha, BOOL32 isOpaque, BOOL32 bb, Matrix4f bbMat);

	virtual ~QuadRenderer();
private:
	Context *context;
};

}

#endif /* QUADRENDERER_H_ */
