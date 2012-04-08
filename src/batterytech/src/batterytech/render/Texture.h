/*
 * Texture.h
 *
 *  Created on: May 16, 2011
 *      Author: rgreen
 */

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
