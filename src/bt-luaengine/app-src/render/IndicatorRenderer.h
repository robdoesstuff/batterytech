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

/*#ifndef INDICATORRENDERER_H_
#define INDICATORRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/Context.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include "BatchSpriteRenderer.h"
#include <batterytech/render/ShaderProgram.h>
#include <batterytech/math/Vector3.h>

using namespace BatteryTech;

class SimpleSpriteRenderer;

#define INDICATOR_MAX_LINES 1000

class IndicatorRenderer: public Renderer {
public:
	IndicatorRenderer(Context *context);
	virtual ~IndicatorRenderer();
	virtual void init(BOOL32 newContext);
    virtual void drawLine(const Vector3f& from,const Vector3f& to,const Vector3f& color);
    void render(World *world);
    void render2D();
private:
	Context *context;
	ShaderProgram *shaderProgram;
	SimpleSpriteRenderer *spriteRenderer;
	F32 vPos[INDICATOR_MAX_LINES * 6];
	F32 vColor[INDICATOR_MAX_LINES * 8];
	S32 lineCount;
};

#endif //INDICATORRENDERER_H_ */
