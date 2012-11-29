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

#ifndef WORLDRENDERER_H_
#define WORLDRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/render/TextRasterRenderer.h>
#include <batterytech/render/GraphicsConfiguration.h>
#include <batterytech/primitives.h>
#include "../World.h"
#include "SimpleSpriteRenderer.h"
#include "ObjRenderer.h"
#include <batterytech/render/Texture.h>
#include <batterytech/util/HashTable.h>
#include "ShadowMap.h"
#include <batterytech/batterytech_entrypoints.h>
#include <batterytech/math/Vector2.h>
#include <batterytech/math/Vector4.h>

class SimpleSpriteRenderer;
class ObjRenderer;
class ScreenControlRenderer;
class AssimpRenderer;
class GameContext;
class ParticleEmitterRenderer;
#ifdef BATTERYTECH_INCLUDE_BOX2D
class B2DebugRenderer;
#endif

namespace BatteryTech { class TextRasterRenderer; }

using namespace BatteryTech;

class WorldRenderer: public Renderer, public BTApplicationRenderer {
public:
	WorldRenderer(GameContext *context);
	void init(BOOL32 newContext);
	void render();
	virtual ~WorldRenderer();
	BOOL32 isLoadingScreenDisplayed() { return loadingScreenDisplayed; }
	void addTextRenderer(const char *tag, TextRasterRenderer *renderer);
	TextRasterRenderer* getTextRenderer(const char *tag);
private:
	void renderLoadingScreen();
	void render3D();
	void render2D();
	void pickLoadingTexture();
	void setupGL();
    U32 sortRenderItemsOfType(U32 renderTypeMask);
    RenderItem **sortedItems;
	SimpleSpriteRenderer *spriteRenderer;
	StrHashTable<TextRasterRenderer*> *textRenderers;
	ObjRenderer *objRenderer;
	AssimpRenderer *assimpRenderer;
	GraphicsConfiguration *gConfig;
	ScreenControlRenderer *screenControlRenderer;
	ParticleEmitterRenderer *particleRenderer;
#ifdef BATTERYTECH_INCLUDE_BOX2D
	B2DebugRenderer *b2DebugRenderer;
#endif
	ShadowMap *shadowMap;
	GameContext *context;
	S32 frameSamplesCollected;
	F32 frameSampleTimeTotal;
	S32 fps;
	BOOL32 loadingScreenDisplayed;
	BOOL32 preLoad;
	Texture *loadingTex;
	Vector2f loadingSize;
	Vector4f clearColor;
};

#endif /* WORLDRENDERER_H_ */
