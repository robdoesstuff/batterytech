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
// Name        : GraphicsConfiguration.h
// Description : The current graphics/rendering configuration
//============================================================================

#ifndef GRAPHICSCONFIGURATION_H_
#define GRAPHICSCONFIGURATION_H_

#include "../primitives.h"

namespace BatteryTech {

	class GraphicsConfiguration {
	public:
		GraphicsConfiguration();
		virtual ~GraphicsConfiguration();
		/**
		 * Often, a game will need to support high and low res textures.
		 */
		enum TextureRes {
			LOW,
			HIGH
		};

		/**
		 * What types of filtering options will you use for your game?
		 */
		enum TextureFilter {
            DEFAULT,
			NONE,
			LINEAR,
			BILINEAR,
			TRILINEAR
		};

		enum ShadowType {
			SHADOWTYPE_NONE,
			SHADOWTYPE_SHADOWMAP,
			SHADOWTYPE_SHADOWMAP_HQ,
			SHADOWTYPE_SHADOWMAP_CUSTOM
		};

		/**
		 * The scale of the UI on the current display.  1.0 = reference size.
		 */
		F32 uiScale;
		/**
		 * The scale of the X-axis on the current display.  1.0 = reference width.
		 */
		F32 scaleX2d;
		/**
		 * The scale of the Y-axis on the current display.  1.0 = reference height;
		 */
		F32 scaleY2d;
		/**
		 * The width of the current display.
		 */
		S32 width;
		/**
		 * The height of the current display.
		 */
		S32 height;
		/**
		 * The width of the current viewport.
		 */
		S32 viewportWidth;
		/**
		 * The height of the current viewport.
		 */
		S32 viewportHeight;
		/**
		 * The texture resolution to load textures at.
		 * This must be implemented by your rendering system.
		 */
		TextureRes textureRes;
		/**
		 * The default filter mode for textures.
		 */
		TextureFilter textureFilter;
        /**
         * The default texture clamp or repeat mode.
         */
        BOOL32 textureRepeat;
		/**
		 * If we're using shaders to render.
		 * Each platform will have a flag to set for using shaders when available.
		 */
		BOOL32 useShaders;

		/**
		 * The type of shadow requested by the application
		 */
		ShadowType shadowType;

		// --------- GPU capabilities ------------
		/**
		 * Does GPU support VBOs?
		 */
		BOOL32 supportsVBOs;
		/**
		 * Does GPU support hardware mipmap generation?
		 */
		BOOL32 supportsHWmipmapgen;
		/**
		 * Does GPU support UV transforms?
		 */
		BOOL32 supportsUVTransform;
		/**
		 * Is GPU a software renderer?
		 */
		BOOL32 softGPU;
		/**
		 * Does GPU support shaders?
		 */
		BOOL32 supportsShaders;
		/**
		 * Does GPU support framebuffer objects?
		 */
		BOOL32 supportsFBOs;
		/**
		 * Does GPU support floating point textures?
		 */
		BOOL32 supportsFloatTextures;
		/**
		 * Does GPU support depth-only textures?
		 */
		BOOL32 supportsDepthTextures;
	};

}
#endif /* GRAPHICSCONFIGURATION_H_ */
