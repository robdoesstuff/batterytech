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
// Name        : AtlasMappedTexture.h
// Description : An OpenGL Texture mapped to an existing Atlas (AssetTexture)
//============================================================================

#ifndef ATLASMAPPEDTEXTURE_H_
#define ATLASMAPPEDTEXTURE_H_

#include "Texture.h"
#include "../math/Vector2.h"
#include "../primitives.h"

namespace BatteryTech {

class Context;
class AssetTexture;

class AtlasMappedTexture : public Texture {
public:
	AtlasMappedTexture(Context *context, AssetTexture *assetTexture, const char *assetName);
	virtual ~AtlasMappedTexture();
	virtual void invalidateGL() {};
	virtual void load(BOOL32 force=FALSE) {};
	virtual void unload() {};
	virtual void bind();
	virtual BOOL32 isLoaded();
	virtual Vector2f getOriginalSize() { return origSize; }
	virtual Vector2f getTrimmedSize() { return trimmedSize; }
	virtual Vector2f getCornerOffset() { return cornerOffset; }
	virtual Matrix4f getMatrix() { return mat; }
    virtual GLuint getTextureId();
	void createMat();
	Context *context;
	AssetTexture *assetTexture;
	Vector2f uvs[4];
	BOOL32 rotated;
	BOOL32 trimmed;
	Vector2i origSize;
	Vector2i cornerOffset;
	Vector2i trimmedSize;
	Matrix4f mat;
};

}

#endif /* ATLASMAPPEDTEXTURE_H_ */
