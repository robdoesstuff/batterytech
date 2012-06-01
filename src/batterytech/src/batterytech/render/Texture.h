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

	/**
	 * \brief An OpenGL Texture
	 *
	 * Implementations of Texture such as AssetTexture load or represent texture data in OpenGL and offer ways to bind and use the texture.
	 */
	class Texture {
	public:
		enum TextureFilter { TEX_FILTER_NEAREST, TEX_FILTER_LINEAR };
		Texture(const char *assetName);
		virtual ~Texture();
		virtual void invalidateGL()=0;
		virtual void load(BOOL32 force=FALSE)=0;
		virtual void unload()=0;
		virtual void bind()=0;
		virtual BOOL32 isLoaded()=0;
		virtual S32 getWidth()=0;
		virtual S32 getHeight()=0;
		static U32 textureSwitches;
		static GLuint lastTextureId;
		char *assetName;
		BOOL32 mipmap;
		TextureFilter magFilter, minFilter;
		BOOL32 repeatX, repeatY;
	};

}
#endif /* TEXTURE_H_ */
