/*
 * Texture.cpp
 *
 *  Created on: May 16, 2011
 *      Author: rgreen
 */

#include "Texture.h"
#include "../util/strx.h"
#include "../platform/platformgeneral.h"
#include "../decoders/stb_image.h"
#include "../Context.h"
#include "../render/GraphicsConfiguration.h"
#include "../render/Renderer.h"
#include "../Logger.h"
#include "../util/BitmapUtil.h"
#include "../batterytech_globals.h"

#define DEBUG_TEXTURE TRUE
#define HALF_SIZE_WIDTH 854
#define HALF_SIZE_HEIGHT 480

namespace BatteryTech {

	U32 Texture::textureSwitches = 0;
	GLuint Texture::lastTextureId = 0;

	Texture::Texture(Context *context, const char *assetName, BOOL32 loadOnDemand) {
		this->context = context;
		this->assetName = strDuplicate(assetName);
		this->loadOnDemand = loadOnDemand;
		textureId = 0;
		mipmap = FALSE;
		mipBlend = FALSE;
		magFilter = TEX_FILTER_LINEAR;
		minFilter = TEX_FILTER_LINEAR;
		repeatX = FALSE;
		repeatY = FALSE;
	}

	Texture::~Texture() {
		delete [] assetName;
	}

	Texture::ktx_header Texture::readKTXHeader(const unsigned char *data) {
		ktx_header header;
		memcpy(&header, data, sizeof(ktx_header));
		return header;
	}

	void Texture::load(BOOL32 force) {
		if (loadOnDemand && !force) {
			// defer
			return;
		}
		if (textureId != 0 && !force) {
			// already loaded
			return;
		}
		S32 x, y, n;
		S32 assetSize = 0;
		unsigned char *fileData = _platform_load_asset(assetName, &assetSize);
		this->width = this->height = 0;
		if (!fileData) {
			char buf[1024];
			sprintf(buf, "No asset data found for %s", assetName);
			logmsg(buf);
			return;
		}
		if (!strContains(assetName, ".ktx") && !strContains(assetName, ".KTX")) {
			unsigned char *dataRaw = stbi_load_from_memory(fileData, assetSize, &x, &y, &n, 0);
			if (dataRaw) {
				BOOL32 scaleDown = FALSE;
				// This is where we flip to half-size textures for smaller screens.
				if (context->gConfig->viewportWidth <= HALF_SIZE_WIDTH && context->gConfig->viewportHeight <= HALF_SIZE_HEIGHT) {
					scaleDown = TRUE;
				}
				int bytes = x * y * n * sizeof(unsigned char);
				unsigned char *data;
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
				}
				if (DEBUG_TEXTURE) {
					char buf[1024];
					sprintf(buf, "Loaded Texture %s (%d enc bytes): %ix%i components=%i bytes=%i", assetName, assetSize, x, y, n, bytes);
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
				if (magFilter == Texture::TEX_FILTER_NEAREST) {
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				} else {
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
				if (minFilter == Texture::TEX_FILTER_NEAREST) {
					if (mipmap) {
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
					} else {
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					}
				} else {
					if (mipmap) {
						if (context->gConfig->textureFilter == GraphicsConfiguration::TRILINEAR) {
							glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						} else {
							glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
						}
					} else {
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					}
				}
				if (n == 3) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				} else if (n == 4) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				}
				if (mipmap) {
					char buf[1024];
					sprintf(buf, "Mipmapping texture %s", assetName);
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
					delete [] data;
				}
				this->width = x;
				this->height = y;
			} else {
				char buf[1024];
				sprintf(buf, "Error decoding %s (%d enc bytes)", assetName, assetSize);
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

	void Texture::invalidateGL() {
		// it means we lost our GL context
		textureId = 0;
	}

	void Texture::unload() {
		if (textureId) {
			glDeleteTextures(1, &textureId);
			textureId = 0;
		}
	}

	void Texture::bind() {
		if (textureId == 0 && loadOnDemand) {
			load(TRUE);
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

}
