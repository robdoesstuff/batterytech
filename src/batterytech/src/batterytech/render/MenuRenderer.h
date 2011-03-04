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
// Name        : MenuRenderer.h
// Description : Renders Menus and UIComponents
//============================================================================

#ifndef MENURENDERER_H_
#define MENURENDERER_H_

#include "GraphicsConfiguration.h"
#include "TextRasterRenderer.h"
#include "../primitives.h"
#include "../platform/platformgl.h"
#include "../ui/UIManager.h"
#include "../util/ManagedArray.h"
#include "Renderer.h"

namespace BatteryTech {

	class UIManager;
	class TextRasterRenderer;
	class Context;

	class MenuRenderer : Renderer {
	public:
		MenuRenderer(Context *context);
		/**
		 * Adds a texture asset or returns the ID of the one already added if equal.
		 * It's acceptable to call this multiple times for the same asset.  It won't allow overlap so that's ok.
		 * This must be called before a UI texture can be rendered.
		 */
		S32 addTextureAsset(const char *asset);
		/**
		 * For rendering - finds a texture asset's ID.
		 */
		S32 findTextureAsset(const char *asset);
		/**
		 * Initializes menu rendering GL
		 */
		virtual void init(BOOL32 newContext);
		/**
		 * Renders menus/UI
		 */
		void render();
		/**
		 * Renders a specific UI component
		 */
		void render(UIComponent *component);
		virtual ~MenuRenderer();
	private:
		void loadTexture(const char *assetName);
		void drawTexturedQuad(S32 top, S32 right, S32 bottom, S32 left);
		Context *context;
		ManagedArray<const char> *assetNames;
		GLuint *textureIds;
		S32 activeResourceId;
		TextRasterRenderer *textRenderer;
		GLuint vertShader, fragShader, program, shaderProjMatrix, shaderMVMatrix, shaderVPosition, shaderUvMap, shaderTex, shaderColorFilter;
	};

}
#endif /* MENURENDERER_H_ */
