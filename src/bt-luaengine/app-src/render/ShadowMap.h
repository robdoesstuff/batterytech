/*
 * ShadowMap.h
 *
 *  Created on: Oct 27, 2011
 *      Author: rgreen
 */

#ifndef SHADOWMAP_H_
#define SHADOWMAP_H_

#include <batterytech/math/Vector3.h>
#include <batterytech/math/Vector4.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/render/GraphicsConfiguration.h>

class GameContext;
using namespace BatteryTech;

class ShadowMap {
public:
	ShadowMap(GameContext *context);
	virtual ~ShadowMap();
	void init(BOOL32 newContext);
	void bindForMapCreation();
	void unbindAfterMapCreation();
	void bindForSceneRender();
	void unbindAfterSceneRender();
	void bindAsTexture();
private:
	void generateShadowFBO();
	GameContext *context;
    GLuint shadowFrameBuffer;
   	GLuint renderBuffer;
    GLuint shadowTexture;
    BOOL32 hasDepthTexture;
    GLuint shadowWidth, shadowHeight;
    GLuint defaultFrameBuffer;
    GraphicsConfiguration::ShadowType currentShadowType;
};

#endif /* ShadowMap_H_ */
