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
	enum ShadowTextureType {
		SHADOWTEXTURE_NONE,
		SHADOWTEXTURE_DEPTH,
		SHADOWTEXTURE_FLOAT,
		SHADOWTEXTURE_RGB,
		SHADOWTEXTURE_ALPHA
	};
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
	/**
	 * The texture type of the shadow implemented by shadow system
	 */
    ShadowTextureType shadowTextureType;
};

#endif /* ShadowMap_H_ */
