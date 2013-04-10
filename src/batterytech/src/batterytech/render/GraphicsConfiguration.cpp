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
// Name        : GraphicsConfiguration.cpp
// Description : The current graphics/rendering configuration
//============================================================================

#include "GraphicsConfiguration.h"

namespace BatteryTech {

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
        textureRepeat = true;
		useShaders = FALSE;
		softGPU = FALSE;
		supportsVBOs = FALSE;
		supportsHWmipmapgen = FALSE;
		supportsUVTransform = FALSE;
		supportsShaders = FALSE;
		supportsFBOs = FALSE;
		supportsFloatTextures = FALSE;
		supportsDepthTextures = FALSE;
	}

	GraphicsConfiguration::~GraphicsConfiguration() {
	}

}
