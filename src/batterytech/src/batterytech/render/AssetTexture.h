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
// Name        : AssetTexture.h
// Description : An Asset-based OpenGL Texture
//============================================================================

#ifndef ASSETTEXTURE_H_
#define ASSETTEXTURE_H_

#include "../primitives.h"
#include "../platform/platformgl.h"
#include "Texture.h"
#include "../util/ManagedArray.h"

static const Vector2i vector2i_0(0,0);

namespace BatteryTech {

	class Context;	

	/**
	 * \brief An Asset-backed OpenGL Texture
	 *
	 * This class loads textures from assets and binds them to OpenGL
	 */
	class AssetTexture : public Texture {
		struct ktx_header {
			char identifier[12];
			GLuint endianness;
			GLuint glType;
			GLuint glTypeSize;
			GLuint glFormat;
			GLuint glInternalFormat;
			GLuint glBaseInternalFormat;
			GLuint pixelWidth;
			GLuint pixelHeight;
			GLuint pixelDepth;
			GLuint numberOfArrayElements;
			GLuint numberOfFaces;
			GLuint numberOfMipmapLevels;
			GLuint bytesOfKeyValueData;
		};

	public:
		AssetTexture(Context *context, const char *assetName, BOOL32 loadOnDemand=FALSE);
		virtual ~AssetTexture();
		virtual void invalidateGL();
		virtual void load(BOOL32 force=FALSE);
		virtual void unload();
		virtual void bind();
		virtual BOOL32 isLoaded();
		virtual Vector2f getOriginalSize() { return size; }
		virtual Vector2f getTrimmedSize() { return size; }
		virtual Vector2f getCornerOffset() { return vector2i_0; }
		virtual Matrix4f getMatrix() { return mat; }
		virtual void clearAliases();
        virtual GLuint getTextureId() { return textureId; }
		static Matrix4f mat;
		GLuint textureId;
		Vector2i size;
		Context *context;
		BOOL32 loadOnDemand;
	private:
		ktx_header readKTXHeader(const unsigned char *data);
		void loadBTXAtlas();
		void loadImageData(const char *imageAssetName);
		ManagedArray<char> *atlasMappedTexureNames;
		void logDebugInfo(const char *message);
		BOOL32 debugTextures;
	};

}
#endif /* ASSETTEXTURE_H_ */
