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
	class ShaderProgram;

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
		/** \brief Gets the asset name of a menu resource ID
		 * \param resourceId the Resource ID
		 * \returns The asset name
		 */
		const char* getAssetNameForResource(S32 resourceId);
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
		/**
		 * Determines if, using available width, this text requires multiple lines
		 */
		BOOL32 isMultiline(const char *text, F32 availableWidth);
		/**
		 * Calculates the total height of the multiline text
		 */
		F32 measureMultilineHeight(const char *text, F32 availableWidth);
		/**
		 * Calculates the width of the given text using current UI font,
		 * may return value greater than available width (switch to multiline then)
		 */
		F32 measureTextWidth(const char *text);
		/**
		 * Gets the height of the current UI font
		 */
		F32 getTextHeight();

		TextRasterRenderer* getTextRenderer() { return textRenderer; }

		virtual ~MenuRenderer();
	private:
		void loadTexture(const char *assetName);
		void drawTexturedQuad(S32 top, S32 right, S32 bottom, S32 left);
		Context *context;
		ManagedArray<const char> *assetNames;
		S32 activeResourceId;
		TextRasterRenderer *textRenderer;
		ShaderProgram *shaderProgram;
	};

}
#endif /* MENURENDERER_H_ */
