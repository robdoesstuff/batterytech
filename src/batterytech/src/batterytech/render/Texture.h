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
// Name        : Texture.h
// Description : An OpenGL Texture
//============================================================================

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "../primitives.h"
#include "../platform/platformgl.h"
#include "../math/Matrix4.h"
#include "GraphicsConfiguration.h"

namespace BatteryTech {

	/**
	 * \brief An OpenGL Texture
	 *
	 * Implementations of Texture such as AssetTexture load or represent texture data in OpenGL and offer ways to bind and use the texture.
	 */
	class Texture {
	public:
		enum TextureFilter { TEX_FILTER_DEFAULT, TEX_FILTER_NEAREST, TEX_FILTER_LINEAR };
		Texture(const char *assetName);
		virtual ~Texture();
		/** \brief Tells this texture that the OpenGL Context is now invalid */
		virtual void invalidateGL()=0;
		/** \brief Loads the texture
		 *
		 * \param force Force the texture to load, overriding any laziness flags
		 *
		 * */
		virtual void load(BOOL32 force=FALSE)=0;
		/** \brief Unloads the texture */
		virtual void unload()=0;
		/** \brief Binds the texture
		 *
		 * Texture bindings are cached to avoid unnecessary binding calls.  Group draws by texture bind and
		 * atlas 2D textures to optimize binding.  You must always bind before drawing.
		 *
		 * */
		virtual void bind()=0;
		/** \brief Is this texture loaded?
		 *
		 * A call to \ref load will load the texture and if successful, this will be set
		 *
		 * \return TRUE if loaded, FALSE if not
		 */
		virtual BOOL32 isLoaded()=0;
		/** \brief returns the original size of this texture
		 *
		 * This is not the actual file size but instead is the original dimension of the image before trim or stretch.
		 *
		 * Scaling this value to screen should give the desired drawing dimensions.
		 *
		 * \return The original dimensions of this image before trim or stretch
		 */
		virtual Vector2f getOriginalSize()=0;
		/** \brief returns the size after trimming
		 *
		 * Used in conjunction with the corner offset, this function will return the dimension that a texture exists as
		 * on an atlas or otherwise where it was saved as a trimmed dimension and not as the original.
		 *
		 * \return The trimmed size
		 */
		virtual Vector2f getTrimmedSize()=0;
		/** \brief returns the offset of the top left corner after trimming
		 *
		 * Trimming an image when placing on an atlas causes an issue where the image most likely was not centered in the alpha
		 * area before trimming.  This number should be used in conjunction with the original size and trimmed size to give
		 * accurate information for layout.
		 *
		 * \return The top left corner offset (delta) between the original size and trimmed size
		 */
		virtual Vector2f getCornerOffset()=0;
		/**
		 * \brief returns a matrix which will transform normal UV coordinates into this texture's UVs.
		 *
		 * A Texture may be virtualized (atlased or otherwise) and this matrix will transform regular UV coordinates
		 * into the real texture's UV space, handling rotation, offset and scale.  Always multiply normal
		 * UVs by this matrix when drawing a texture that may be atlased.
		 *
		 * \return The texture matrix
		 */
		virtual Matrix4f getMatrix()=0;
		/**
		 * \brief instructs this texture to callback and clear any alias textures it created on its last load
		 *
		 * Alias textures are things like AtlasMappedTexture
		 *
		 * Care needs to be taken when calling functions like this as they need to not happen while iterating the entire array of textures
		 */
		virtual void clearAliases() {};
        /**
         * \brief gets the texture ID
         *
         * \return The ID of this texture
         */
        virtual GLuint getTextureId()=0;
		static U32 textureSwitches;
		static GLuint lastTextureId;
		char *assetName;
		BOOL32 mipmap;
        GraphicsConfiguration::TextureFilter filter;
		TextureFilter magFilter, minFilter;
		BOOL32 repeatX, repeatY;
	};

}
#endif /* TEXTURE_H_ */
