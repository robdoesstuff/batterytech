/*
 * GraphicsConfiguration.cpp
 *
 *  Created on: Sep 5, 2010
 *      Author: rgreen
 */

#include "GraphicsConfiguration.h"

GraphicsConfiguration::GraphicsConfiguration() {
	uiScale = 0;
	scaleX2d = 0;
	scaleY2d = 0;
	width = 0;
	height = 0;
	viewportHeight = 0;
	viewportWidth = 0;
	textureRes = HIGH;
	textureFilter = LINEAR;
	showFPS = FALSE;
	useShaders = FALSE;
	softGPU = FALSE;
	supportsVBOs = FALSE;
	supportsHWmipmapgen = FALSE;
	supportsUVTransform = FALSE;
	supportsShaders = FALSE;
}

GraphicsConfiguration::~GraphicsConfiguration() {
}
