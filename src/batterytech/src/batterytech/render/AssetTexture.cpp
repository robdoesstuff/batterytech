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
// Name        : AssetTexture.cpp
//============================================================================

#include "AssetTexture.h"
#include "../util/strx.h"
#include "../platform/platformgeneral.h"
#include "../decoders/stb_image.h"
#include "../Context.h"
#include "../render/GraphicsConfiguration.h"
#include "../render/Renderer.h"
#include "../Logger.h"
#include "../util/BitmapUtil.h"
#include "../batterytech_globals.h"
#include "../util/TextFileUtil.h"
#include "AtlasMappedTexture.h"
#include "GLResourceManager.h"

#define MAX_ATLAS_MAPPED_TEXTURES 50

namespace BatteryTech {

	// define our static matrix
	Matrix4f AssetTexture::mat;

	AssetTexture::AssetTexture(Context *context, const char *assetName, BOOL32 loadOnDemand) : Texture(assetName) {
		this->context = context;
		this->loadOnDemand = loadOnDemand;
		textureId = 0;
		atlasMappedTexureNames = new ManagedArray<char>(MAX_ATLAS_MAPPED_TEXTURES);
	}

	AssetTexture::~AssetTexture() {
		atlasMappedTexureNames->deleteElements();
		delete atlasMappedTexureNames;
		atlasMappedTexureNames = NULL;
	}

	AssetTexture::ktx_header AssetTexture::readKTXHeader(const unsigned char *data) {
		ktx_header header;
		memcpy(&header, data, sizeof(ktx_header));
		return header;
	}

	void AssetTexture::load(BOOL32 force) {
		if (loadOnDemand && !force) {
			// defer
			return;
		}
		if (textureId != 0 && !force) {
			// already loaded
			return;
		}
		if (strContains(assetName, ".btx") || strContains(assetName, ".BTX")) {
			loadBTXAtlas();
		} else {
			loadImageData(assetName);
		}
	}

	void AssetTexture::loadBTXAtlas() {
		Property *prop = context->appProperties->get("debug_textures");
		BOOL32 debugTextures = FALSE;
		if (prop) {
			debugTextures = prop->getBoolValue();
		}
		char *data = _platform_load_text_asset(assetName);
		if (!data) {
			return;
		}
		char lineBuf[1024];
		char keyBuf[64];
		char valueBuf[1024];
		S32 dataPos = 0;
		char texAssetName[1024];
		char basePath[512];
		char logbuf[512];
		// convert assetName to our path
		_platform_convert_path(assetName, basePath);
		_platform_get_basename(basePath, basePath);
		// add path separator IE: "/ui/"
		strcat(basePath, _platform_get_path_separator());
		// now let's do this in forward-slash so we don't have to do any more converting
		_platform_convert_path_to_forward(basePath, basePath);
		AtlasMappedTexture *vTex = NULL;
		while (TextFileUtil::readLine(lineBuf, data, &dataPos)) {
			// parse key/value from line
			char *untrimmedkey = strtok(lineBuf, "=");
			char *untrimmedval = strtok(NULL, "=");
			S32 length = 0;
			char *trimmedKey = strTrim(untrimmedkey, &length);
			strncpy(keyBuf, trimmedKey, length);
			keyBuf[length] = '\0';
			char *trimmedValue = strTrim(untrimmedval, &length);
			strncpy(valueBuf, trimmedValue, length);
			valueBuf[length] = '\0';
			// now interpret the key and cast the value
			if (strEquals(keyBuf, "btx")) {
				// right now we only support btx 1.0
				if (!strEquals(valueBuf, "1.0")) {
					logmsg("Unsupported version of btx!  This version of BatteryTech SDK only supports BTX 1.0");
					break;
				}
			} else if (strEquals(keyBuf, "texture.assetname")) {
				// add path to btx onto assetname
				strcpy(texAssetName, basePath);
				strcat(texAssetName, valueBuf);
				loadImageData(texAssetName);
			} else if (strEquals(keyBuf, "image.assetname")) {
				// finish up previous one
				if (vTex) {
					if (debugTextures) {
						sprintf(logbuf, "Atlas Mapped Texture %s orig size %d %d tsize %d %d offsets %d %d", vTex->assetName, vTex->origSize.x, vTex->origSize.y, vTex->trimmedSize.x, vTex->trimmedSize.y, vTex->cornerOffset.x, vTex->cornerOffset.y);
						logmsg(logbuf);
					}
					vTex->createMat();
				}
				// add path to btx onto assetname
				strcpy(texAssetName, basePath);
				strcat(texAssetName, valueBuf);
				vTex = new AtlasMappedTexture(context, this, texAssetName);
				Texture *existingTex = context->glResourceManager->getTexture(texAssetName);
				if (existingTex) {
					// Already has one!  remove/override.
					context->glResourceManager->removeTexture(texAssetName);
				}
				context->glResourceManager->addTexture(vTex);
				atlasMappedTexureNames->add(strDuplicate(texAssetName));
			} else if (strEquals(keyBuf, "image.uvs")) {
				char *u1 = strtok(valueBuf, " ");
				char *v1 = strtok(NULL, " ");
				char *u2 = strtok(NULL, " ");
				char *v2 = strtok(NULL, " ");
				char *u3 = strtok(NULL, " ");
				char *v3 = strtok(NULL, " ");
				char *u4 = strtok(NULL, " ");
				char *v4 = strtok(NULL, " ");
				vTex->uvs[0] = Vector2f(atof(u1), atof(v1));
				vTex->uvs[1] = Vector2f(atof(u2), atof(v2));
				vTex->uvs[2] = Vector2f(atof(u3), atof(v3));
				vTex->uvs[3] = Vector2f(atof(u4), atof(v4));
			} else if (strEquals(keyBuf, "image.rotated")) {
				if (strEquals(valueBuf, "true")) {
					vTex->rotated = TRUE;
				} else {
					vTex->rotated = FALSE;
				}
			} else if (strEquals(keyBuf, "image.trimmed")) {
				if (strEquals(valueBuf, "true")) {
					vTex->trimmed = TRUE;
				} else {
					vTex->trimmed = FALSE;
				}
			} else if (strEquals(keyBuf, "image.origsize")) {
				char *width = strtok(valueBuf, " ");
				char *height = strtok(NULL, " ");
				vTex->origSize = Vector2i(atoi(width), atoi(height));
			} else if (strEquals(keyBuf, "image.offset")) {
				char *x = strtok(valueBuf, " ");
				char *y = strtok(NULL, " ");
				vTex->cornerOffset = Vector2i(atoi(x), atoi(y));
			} else if (strEquals(keyBuf, "image.trimmedsize")) {
				char *width = strtok(valueBuf, " ");
				char *height = strtok(NULL, " ");
				vTex->trimmedSize = Vector2i(atoi(width), atoi(height));
			}
		}
		// and now last one
		if (vTex) {
			if (debugTextures) {
				sprintf(logbuf, "Atlas Mapped Texture %s orig size %d %d tsize %d %d offsets %d %d", vTex->assetName, vTex->origSize.x, vTex->origSize.y, vTex->trimmedSize.x, vTex->trimmedSize.y, vTex->cornerOffset.x, vTex->cornerOffset.y);
				logmsg(logbuf);
			}
			vTex->createMat();
		}
		_platform_free_asset((unsigned char*)data);
	}

	void AssetTexture::invalidateGL() {
		// it means we lost our GL context
		textureId = 0;
	}

	void AssetTexture::unload() {
		if (textureId) {
            clearAliases();
			glDeleteTextures(1, &textureId);
			textureId = 0;
		}
	}

	void AssetTexture::bind() {
		if (textureId == 0 && loadOnDemand) {
			load(TRUE);
		}
		if (textureId == 0) {
			char buf[1024];
			sprintf(buf, "Texture not loaded %s", assetName);
			logmsg(buf);
		}
		if (Texture::lastTextureId != textureId) {
			Texture::textureSwitches++;
			glBindTexture(GL_TEXTURE_2D, textureId);
			Texture::lastTextureId = textureId;
		}
		//char buf[1024];
		//sprintf(buf, "Bound to textureId %d", textureId);
		//logmsg(buf);
	}

	BOOL32 AssetTexture::isLoaded() {
		return (textureId != 0);
	}

	void AssetTexture::clearAliases() {
		for (S32 i = 0; i < atlasMappedTexureNames->getSize(); i++) {
			context->glResourceManager->removeTexture(atlasMappedTexureNames->array[i]);
		}
		atlasMappedTexureNames->deleteElements();
	}

	void AssetTexture::loadImageData(const char *imageAssetName) {
		Property *prop = context->appProperties->get("debug_textures");
		debugTextures = FALSE;
		if (prop) {
			debugTextures = prop->getBoolValue();
		}
		S32 halfWidthThreshold = 0;
		S32 halfHeightThreshold = 0;
		prop = context->appProperties->get("texture_half_width_threshold");
		if (prop) {
			halfWidthThreshold = prop->getIntValue();
		}
		prop = context->appProperties->get("texture_half_height_threshold");
		if (prop) {
			halfHeightThreshold = prop->getIntValue();
		}
		S32 x, y, n;
		S32 assetSize = 0;
		unsigned char *fileData = _platform_load_asset(imageAssetName, &assetSize);
		this->size = Vector2i(0,0);
		if (!fileData) {
			char buf[1024];
			sprintf(buf, "No asset data found for %s", imageAssetName);
			logmsg(buf);
			return;
		}
		if (!strContains(imageAssetName, ".ktx") && !strContains(imageAssetName, ".KTX")) {
			unsigned char *dataRaw = stbi_load_from_memory(fileData, assetSize, &x, &y, &n, 0);
			if (dataRaw) {
				BOOL32 scaleDown = FALSE;
				// This is where we flip to half-size textures for smaller screens.
				if (context->gConfig->viewportWidth <= halfWidthThreshold || context->gConfig->viewportHeight <= halfHeightThreshold) {
					scaleDown = TRUE;
				}
				int bytes = x * y * n * sizeof(unsigned char);
				unsigned char *data = NULL;
				unsigned short *shortData = NULL;
				if (FORCE_SQUARE_TEXTURES || scaleDown) {
					if (FORCE_SQUARE_TEXTURES) {
						int newX = x;
						int newY = y;
						if (x > y) {
							newX = newY = x;
						} else {
							newX = newY = y;
						}
						if (scaleDown) {
							newX /= 2;
							newY /= 2;
						}
						bytes = newX * newY * n;
						data = new unsigned char[bytes];
						BitmapUtil::roughScale(dataRaw, data, n, x, y, newX, newY);
						x = newX;
						y = newY;
					} else {
						bytes /= 4;
						data = new unsigned char[bytes];
						x /= 2;
						y /= 2;
						BitmapUtil::scaleDown(dataRaw, data, n, x, y);
					}
				} else {
					data = dataRaw;
				}
				if (FORCE_32_BIT_TEXTURES) {
					if (n == 3) {
						n = 4;
						bytes = x * y * n;
						unsigned char *newData = new unsigned char[bytes];
						for (S32 i = 0; i < x*y; i++) {
							newData[i*4] = data[i*3];
							newData[i*4+1] = data[i*3+1];
							newData[i*4+2] = data[i*3+2];
							newData[i*4+3] = 0xFF;
						}
						if (scaleDown) {
							delete [] data;
						}
						scaleDown = true;
						data = newData;
					}
				} else if (FORCE_16_BIT_RGB_TEXTURES) {
					if (n == 3) {
						bytes = x * y * 2;
						shortData = new unsigned short[bytes/2];
						for (S32 i = 0; i < x*y; i++) {
							unsigned char r8 = data[i*3];
							unsigned char g8 = data[i*3+1];
							unsigned char b8 = data[i*3+2];
							shortData[i] = ((unsigned short)(0xF8 & r8) << 8) | ((unsigned short)(0xFC & g8) << 3) | ((unsigned short)(0xF8 & b8) >> 3);
						}
						if (scaleDown) {
							delete [] data;
						}
						data = NULL;
						scaleDown = true;
					}
				}
				if (n == 1) {
					// convert Alpha to RGBA
					n = 4;
					bytes = x * y * n;
					unsigned char *newData = new unsigned char[bytes];
					for (S32 i = 0; i < x*y; i++) {
						newData[i*4] = 255;
						newData[i*4+1] = 255;
						newData[i*4+2] = 255;
						newData[i*4+3] = data[i];
					}
					if (scaleDown) {
						delete [] data;
					}
					scaleDown = true;
					data = newData;
				}
				if (debugTextures) {
					char buf[1024];
					sprintf(buf, "Loaded Texture %s (%d enc bytes): %ix%i components=%i bytes=%i", imageAssetName, assetSize, x, y, n, bytes);
					logmsg(buf);
				}
				GLuint textureIds[1];
				glGenTextures(1, textureIds);
				textureId = textureIds[0];
				Texture::textureSwitches++;
				glBindTexture(GL_TEXTURE_2D, textureId);
				Texture::lastTextureId = textureId;
				if (!context->gConfig->useShaders) {
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				}
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeatX ? GL_REPEAT : GL_CLAMP_TO_EDGE);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeatY ? GL_REPEAT : GL_CLAMP_TO_EDGE);
                // if it's default and default is no filter OR mag is override to nearest
                GraphicsConfiguration::TextureFilter filter = this->filter;
                if (filter == GraphicsConfiguration::DEFAULT) {
                    filter = context->gConfig->textureFilter;
                }
                // Some quick terminology:
                // Nearest neighbor filtering or "None" as we call it is Min = Nearest, Mag = Nearest
                // Linear filtering is Min = Nearest, Mag = Linear
                // Bilinear filtering is Min = Linear OR Linear_mipmap_nearest, Mag = Linear
                // Trilinear filtering is Min = Linear OR Linear_mipmap_linear, Mag = Linear
                if ((magFilter == Texture::TEX_FILTER_DEFAULT && filter == GraphicsConfiguration::NONE) || magFilter == Texture::TEX_FILTER_NEAREST) {
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					logDebugInfo("Mag Filter = Nearest");
				} else {
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					logDebugInfo("Mag Filter = Linear");
				}
                // if min filter is default and default is no filter OR min is override to nearest
				if ((minFilter == Texture::TEX_FILTER_DEFAULT && (filter == GraphicsConfiguration::NONE || filter == GraphicsConfiguration::LINEAR)) || minFilter == Texture::TEX_FILTER_NEAREST) {
					if (mipmap && data) {
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
						logDebugInfo("Min Filter = Nearest Mipmap Nearest");
					} else {
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						logDebugInfo("Min Filter = Nearest");
					}
				} else {
					if (mipmap && data) {
						if (filter == GraphicsConfiguration::TRILINEAR) {
							glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
							logDebugInfo("Min Filter = Linear Mipmap Linear");
						} else {
							glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
							logDebugInfo("Min Filter = Linear Mipmap Nearest");
						}
					} else {
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						logDebugInfo("Min Filter = Linear");
					}
				}
				if (n == 3) {
					if (FORCE_16_BIT_RGB_TEXTURES) {
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, shortData);
					} else {
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					}
				} else if (n == 4) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				}
				// TODO - we don't currently support mipmapping 16 bit textures - need color conversion
				if (mipmap && data) {
					char buf[1024];
					sprintf(buf, "Mipmapping texture %s", imageAssetName);
					logmsg(buf);
					int mipLevel = 0;
					unsigned char *mipData = new unsigned char[bytes/4];
					unsigned char *lastMipData = new unsigned char[bytes];
					memcpy(lastMipData, data, bytes);
					while (x > 0 && y > 0) {
						mipLevel++;
						x /= 2;
						y /= 2;
						BitmapUtil::scaleDown(lastMipData, mipData, n, x, y);
						if (n == 3) {
							glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, mipData);
						} else if (n == 4) {
							glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, mipData);
						}
						memcpy(lastMipData, mipData, x * y * n);
					}
					delete [] mipData;
					delete [] lastMipData;
				}
				Renderer::checkGLError("Renderer Load Texture");
				stbi_image_free(dataRaw);
				if (scaleDown) {
					if (data) {
						delete [] data;
					}
					if (shortData) {
						delete [] shortData;
					}
				}
				// this really isn't a good size to store because it's either an atlas or a stretched PoT image size
				// what we should really do is override this with values provided by the user for the desires draw size
				this->size.x = x;
				this->size.y = y;
			} else {
				char buf[1024];
				sprintf(buf, "Error decoding %s (%d enc bytes)", imageAssetName, assetSize);
				logmsg(buf);
			}
		} else {
			// load compressed texture direct from disk (.ktx file format which is ETC1)
			ktx_header header = readKTXHeader(fileData);
			GLuint textureIds[1];
			glGenTextures(1, textureIds);
			textureId = textureIds[0];
			Texture::textureSwitches++;
			glBindTexture(GL_TEXTURE_2D, textureId);
			Texture::lastTextureId = textureId;
			if (!context->gConfig->useShaders) {
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeatX ? GL_REPEAT : GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeatY ? GL_REPEAT : GL_CLAMP_TO_EDGE);
			// TODO - handle filtering/mipmapping per-texture
			if (context->gConfig->textureFilter == GraphicsConfiguration::NONE) {
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			} else {
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			const unsigned char *dataStart = fileData+sizeof(header)+header.bytesOfKeyValueData;
			U32 dataSize = (U32)assetSize - (sizeof(header)+header.bytesOfKeyValueData);
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, header.glInternalFormat, header.pixelWidth, header.pixelHeight, 0, dataSize, dataStart);
			Renderer::checkGLError("Renderer Load Compressed Texture");
		}
		_platform_free_asset(fileData);
	}

	void AssetTexture::logDebugInfo(const char *message) {
		if (debugTextures) {
			logmsg(message);
		}
	}
}
