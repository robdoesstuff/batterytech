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
// Name        : GraphicsConfiguration.h
// Description : The current graphics/rendering configuration
//============================================================================

#ifndef GRAPHICSCONFIGURATION_H_
#define GRAPHICSCONFIGURATION_H_

#include "../primitives.h"

class GraphicsConfiguration {
public:
	GraphicsConfiguration();
	virtual ~GraphicsConfiguration();
	enum TextureRes {
		LOW,
		HIGH
	};

	enum TextureFilter {
		NONE,
		LINEAR,
		BILINEAR,
		TRILINEAR
	};

	F32 uiScale;
	F32 scaleX2d;
	F32 scaleY2d;
	S32 width;
	S32 height;
	S32 viewportWidth;
	S32 viewportHeight;
	TextureRes textureRes;
	TextureFilter textureFilter;
	BOOL32 showFPS;
	BOOL32 useShaders;
	// capabilities
	BOOL32 supportsVBOs;
	BOOL32 supportsHWmipmapgen;
	BOOL32 supportsUVTransform;
	BOOL32 softGPU;
	BOOL32 supportsShaders;
};

#endif /* GRAPHICSCONFIGURATION_H_ */
