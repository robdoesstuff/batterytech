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
// Name        : Texture.h
// Description : An OpenGL Texture
//============================================================================

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "../primitives.h"
#include "../platform/platformgl.h"

namespace BatteryTech {

	class Context;

	class Texture {
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
		Texture(Context *context, const char *assetName, BOOL32 loadOnDemand=FALSE);
		virtual ~Texture();
		void invalidateGL();
		void load(BOOL32 force=FALSE);
		void unload();
		void bind();
		char *assetName;
		GLuint textureId;
		BOOL32 mipmap;
		BOOL32 mipBlend;
		TextureFilter magFilter, minFilter;
		BOOL32 repeatX, repeatY;
		size_t width, height;
		static U32 textureSwitches;
		static GLuint lastTextureId;
		Context *context;
		BOOL32 loadOnDemand;
	private:
		ktx_header readKTXHeader(const unsigned char *data);
	};

}
#endif /* TEXTURE_H_ */
