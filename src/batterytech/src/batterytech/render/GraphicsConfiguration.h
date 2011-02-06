/*
 * GraphicsConfiguration.h
 *
 *  Created on: Sep 5, 2010
 *      Author: rgreen
 */

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
	// capabilities
	BOOL32 supportsVBOs;
	BOOL32 supportsHWmipmapgen;
	BOOL32 supportsUVTransform;
	BOOL32 softGPU;
};

#endif /* GRAPHICSCONFIGURATION_H_ */
