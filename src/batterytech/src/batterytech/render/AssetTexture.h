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
// Name        : AssetTexture.h
// Description : An Asset-based OpenGL Texture
//============================================================================

#ifndef ASSETTEXTURE_H_
#define ASSETTEXTURE_H_

#include "../primitives.h"
#include "../platform/platformgl.h"
#include "Texture.h"

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
		enum TextureFilter { TEX_FILTER_NEAREST, TEX_FILTER_LINEAR };
		AssetTexture(Context *context, const char *assetName, BOOL32 loadOnDemand=FALSE);
		virtual ~AssetTexture();
		virtual void invalidateGL();
		virtual void load(BOOL32 force=FALSE);
		virtual void unload();
		virtual void bind();
		virtual BOOL32 isLoaded();
		virtual S32 getWidth() { return width; }
		virtual S32 getHeight() { return height; }
		GLuint textureId;
		size_t width, height;
		static U32 textureSwitches;
		static GLuint lastTextureId;
		Context *context;
		BOOL32 loadOnDemand;
	private:
		ktx_header readKTXHeader(const unsigned char *data);
		void loadBTXAtlas(const char *data);
	};

}
#endif /* ASSETTEXTURE_H_ */
