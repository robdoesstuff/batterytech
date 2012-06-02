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
// Name        : AtlasMappedTexture.cpp
//============================================================================

#include "AtlasMappedTexture.h"
#include "AssetTexture.h"
#include "../Context.h"

namespace BatteryTech {

AtlasMappedTexture::AtlasMappedTexture(Context *context, AssetTexture *assetTexture, const char *assetName) : Texture(assetName) {
	this->context = context;
	this->assetTexture = assetTexture;
}

AtlasMappedTexture::~AtlasMappedTexture() {
	context = NULL;
	assetTexture = NULL;
}

void AtlasMappedTexture::bind() {
	if (assetTexture) {
		assetTexture->bind();
	}
}

BOOL32 AtlasMappedTexture::isLoaded() {
	return assetTexture->isLoaded();
}

void AtlasMappedTexture::createMat() {
	mat = Matrix4f;
	// TODO use the UVS and rotation to create a transform matrix
	// Then have anything using UVS run uvs through this matrix using the mult, but we can start with 2D and add 3D later when we want to modify the shaders.
}

}
