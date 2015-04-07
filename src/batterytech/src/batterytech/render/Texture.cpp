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
// Name        : Texture.cpp
// Description : An OpenGL Texture
//============================================================================

#include "Texture.h"
#include "../util/strx.h"

namespace BatteryTech {

	U32 Texture::textureSwitches = 0;
	GLuint Texture::lastTextureId = 0;

	Texture::Texture(const char *assetName) {
		this->assetName = strDuplicate(assetName);
		mipmap = FALSE;
        filter = GraphicsConfiguration::DEFAULT;
		magFilter = TEX_FILTER_DEFAULT;
		minFilter = TEX_FILTER_DEFAULT;
		repeatX = TRUE;
		repeatY = TRUE;
	}

	Texture::~Texture() {
		delete [] assetName;
	}
}
