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
// Name        : AtlasMappedTexture.cpp
//============================================================================

#include "AtlasMappedTexture.h"
#include "AssetTexture.h"
#include "../Context.h"
#include "../Logger.h"
#include <stdio.h>

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
	// use the UVS and rotation to create a transform matrix
	mat.identity();
	if (rotated) {
		// rotate matrix 90 clockwise
		mat.data[0] = 0;
		mat.data[1] = -1;
		mat.data[4] = 1;
		mat.data[5] = 0;
		mat.scale(uvs[0].y - uvs[1].y, uvs[2].x - uvs[1].x, 1.0f);
	} else {
		mat.scale(uvs[1].x - uvs[0].x, uvs[2].y - uvs[0].y, 1.0f);
	}
	mat.data[12] = uvs[0].x;
	mat.data[13] = uvs[0].y;
	/*
	// testing the UV transform
	Vector2f t1(0, 0);
	Vector2f t2(.5f, .5f);
	Vector2f t3(1, 1);
	t1 = mat * t1;
	t2 = mat * t2;
	t3 = mat * t3;
	char buf[100];
	sprintf(buf, "uvs0 = %f %f, uvs2 = %f %f", uvs[0].x, uvs[0].y, uvs[2].x, uvs[2].y);
	logmsg(buf);
	sprintf(buf, "t1 = %f %f t2 = %f %f t3 = %f %f", t1.x, t1.y, t2.x, t2.y, t3.x, t3.y);
	logmsg(buf);
	*/
	// Then have anything using UVS run uvs through this matrix using the mult, but we can start with 2D and add 3D later when we want to modify the shaders.
}
    
    GLuint AtlasMappedTexture::getTextureId() { 
        return assetTexture->getTextureId(); 
    }
}
