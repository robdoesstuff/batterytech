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

#ifndef BATCHSPRITERENDERER_H_
#define BATCHSPRITERENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/Context.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/render/ShaderProgram.h>

using namespace BatteryTech;

class BatchSpriteRenderer : Renderer {
public:
	BatchSpriteRenderer(Context *context, const char *spriteAssetName);
	virtual void init(BOOL32 newContext);
	virtual void startBatch();
	virtual void endBatch();
	virtual void render(F32 top, F32 right, F32 bottom, F32 left);
	virtual void render(F32 x, F32 y, F32 width, F32 height, F32 angle);
	virtual ~BatchSpriteRenderer();
	void setSpriteAssetname(const char* spriteAssetName);
private:
	Context *context;
	const char *spriteAssetName;
	GLuint textureId;
};

#endif /* SIMPLESPRITERENDERER_H_ */
