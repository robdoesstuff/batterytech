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
// Name        : TextRasterRenderer.cpp
// Description : Renders text from a font-baked texture
//============================================================================

#include "TextRasterRenderer.h"
#include "../platform/platformgeneral.h"
#include "../platform/platformgl.h"
#include "../util/strx.h"
#include "../primitives.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "../decoders/stb_truetype.h"
#include "../Logger.h"
#include "../Context.h"
#include "../render/RenderContext.h"
#include "../batterytech_globals.h"
#include <stdio.h>
#include "ShaderProgram.h"
#include <batterytech/util/strx.h>
#include "Texture.h"
#include "GLResourceManager.h"
#include "../util/TextFileUtil.h"
#include "../util/strx.h"
#include "AssetTexture.h"

// uncomment this if there are strange text artifacts - it will put debuggable values into the text arrays which
// will show any algorithm errors in layout if any array positions are skipped accidentally.
// #define DEBUG_TEXT_ELEMENT_ARRAY

#define SHADER_PROGRAM_TAG "quad"
#define USE_VBOS_WHEN_AVAILBLE FALSE

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))

namespace BatteryTech {

	TextRasterRenderer::TextRasterRenderer(Context *context, const char *assetName, F32 fontSize) {
		this->context = context;
		this->assetName = strDuplicate(assetName);
		this->fontSize = fontSize;
		bmpWidth = 0;
		bmpHeight = 0;
		innerStroke = 0;
		outerStroke = 0;
		color = Vector4f(1,1,1,1);
        vertVBOId = 0;
        idxVBOId = 0;
        isTTF = FALSE;
        bufferUsed = 0;
        bmInfo = NULL;
        bmFontPage = NULL;
        bmFontPageTable = NULL;
        bmFontCharTable = NULL;
        bmKerningTable = NULL;
        bmCurrentPage = 0;
        ftex = 0;
        vertBuffer = new GLQuadVertex[TEXT_RENDER_BUFFER_SIZE * 4];
        idxBuffer = new U16[TEXT_RENDER_BUFFER_SIZE*6];
        // generate the face indices
        for (S32 i = 0; i < TEXT_RENDER_BUFFER_SIZE; i++) {
            S32 ii = i*6;
            S32 jj = i*4;
            idxBuffer[ii+0] = jj+0;
            idxBuffer[ii+1] = jj+1;
            idxBuffer[ii+2] = jj+2;
            idxBuffer[ii+3] = jj+0;
            idxBuffer[ii+4] = jj+2;
            idxBuffer[ii+5] = jj+3;
        }
	}

	TextRasterRenderer::~TextRasterRenderer() {
		delete [] assetName;
        delete [] vertBuffer;
        delete [] idxBuffer;
        if (bmInfo) {
            delete bmInfo;
            bmInfo = NULL;
        }
        if (bmFontCharTable) {
            bmFontCharTable->deleteElements();
            delete bmFontCharTable;
            bmFontCharTable = NULL;
        }
        if (bmFontPageTable) {
            bmFontPageTable->deleteElements();
            delete bmFontPageTable;
            bmFontPageTable = NULL;
        }
        if (bmFontPage) {
            delete bmFontPage;
            bmFontPage = NULL;
        }
        if (bmKerningTable) {
        	bmKerningTable->deleteElements();
        	delete bmKerningTable;
        	bmKerningTable = NULL;
        }
	}

	void TextRasterRenderer::setStroke(S32 inner, S32 outer) {
		innerStroke = inner * context->gConfig->uiScale;
		outerStroke = outer * context->gConfig->uiScale;
	}

	void TextRasterRenderer::setColorFilter(Vector4f rgba) {
		color = rgba;
	}

    // TTF
    void TextRasterRenderer::loadTTF() {
		Property *prop = context->appProperties->get("debug_fonts");
		BOOL32 debugFonts = FALSE;
		if (prop) {
			debugFonts = prop->getBoolValue();
		}
		S32 bmpWidth = context->appProperties->get("initial_font_texture_width")->getIntValue();
		S32 bmpHeight = context->appProperties->get("initial_font_texture_height")->getIntValue();
		S32 size = 0;
		BYTE8 *data;
		data = _platform_load_asset(assetName, &size);
		if (data) {
			BYTE8 *temp_bitmap = NULL;
			BYTE8 *temp_bitmap_rgba = NULL;
			BOOL32 fit = FALSE;
			BOOL32 increaseToggle = FALSE;
			S32 ascent, descent, lineGap, minLeftSideBearing, minRightSideBearing, xMaxExtent;
			U32 advanceWidthMax;
			stbtt_fontinfo f;
			stbtt_InitFont(&f, data, 0);
			stbtt_GetFontVMetrics(&f, &ascent, &descent, &lineGap);
			stbtt_GetFontHMetrics(&f, &advanceWidthMax, &minLeftSideBearing, &minRightSideBearing, &xMaxExtent);
			F32 scaledFontSize = fontSize * context->gConfig->uiScale;
			F32 scale = stbtt_ScaleForPixelHeight(&f, scaledFontSize);
			char buf[1024];
			if (debugFonts) {
				sprintf(buf, "Font Metrics: ascent=%d, descent=%d, lineGap=%d, awm=%d, mlsb=%d, mrsb=%d, xmax=%d\n", ascent, descent, lineGap, advanceWidthMax, minLeftSideBearing, minRightSideBearing, xMaxExtent);
				logmsg(buf);
				printf("Bitmap Scale: %f = %f, awmScaled=%f, xmaxScaled=%f\n", scaledFontSize, scale, advanceWidthMax*scale, xMaxExtent*scale);
				logmsg(buf);
			}
			while (!fit) {
				// this isn't the most efficient way to do this but anything better will require modifying stb_truetype to do a dry run with no allocation
				temp_bitmap = (unsigned char*) malloc(sizeof(unsigned char) * bmpWidth*bmpHeight);
				temp_bitmap_rgba = (unsigned char*) malloc(sizeof(unsigned char) * bmpWidth*bmpHeight*4);
				S32 result = stbtt_BakeFontBitmap(data, 0, scaledFontSize, temp_bitmap, bmpWidth, bmpHeight, 32,95, cdata, outerStroke); // no guarantee this fits!
				if (result <= 0) {
					// didn't fit it all
					increaseToggle = !increaseToggle;
					if (increaseToggle) {
						bmpWidth *= 2;
					} else {
						bmpHeight *= 2;
					}
					free(temp_bitmap_rgba);
					free(temp_bitmap);
				} else {
					fit = TRUE;
					this->bmpWidth = bmpWidth;
					this->bmpHeight = bmpHeight;
				}
			}
			// the bitmap we get from the stb lib is 8 bit alpha, so we need to convert it to rgba 32 bit for gl
			S32 row,col;
			for (row = 0; row < bmpHeight; row++) {
				for (col = 0; col < bmpWidth; col++) {
					S32 rowStart = row * bmpWidth * 4;
					BYTE8 alpha = temp_bitmap[row * bmpWidth + col];
					// will give a nice b&w gradient
					temp_bitmap_rgba[rowStart + col * 4] = alpha;
					temp_bitmap_rgba[rowStart + col * 4 + 1] = alpha;
					temp_bitmap_rgba[rowStart + col * 4 + 2] = alpha;
					temp_bitmap_rgba[rowStart + col * 4 + 3] = alpha;
				}
			}
			if (innerStroke) {
				applyInnerStroke(temp_bitmap, temp_bitmap_rgba, bmpWidth, bmpHeight);
			}
			if (outerStroke) {
				applyOuterStroke(temp_bitmap, temp_bitmap_rgba, bmpWidth, bmpHeight);
			}
            // applyAA(temp_bitmap_rgba, bmpWidth, bmpHeight);
			// can free ttf_buffer at this point
			free(temp_bitmap);
			// uncomment for debugging font texture generation
			/**
             FILE *testOutFile;
             testOutFile = fopen("bmp8.raw", "wb");
             fwrite(temp_bitmap, bmpWidth * bmpHeight, 1, testOutFile);
             fclose(testOutFile);
             testOutFile = fopen("bmp32.raw", "wb");
             fwrite(temp_bitmap_rgba, bmpWidth * bmpHeight * 4, 1, testOutFile);
             fclose(testOutFile);
             */
			glGenTextures(1, &ftex);
			Texture::textureSwitches++;
			glBindTexture(GL_TEXTURE_2D, ftex);
			Texture::lastTextureId = ftex;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmpWidth, bmpHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_bitmap_rgba);
			free(temp_bitmap_rgba);
			checkGLError("loadTTF");
			// can free temp_bitmap at this point
			_platform_free_asset(data);
		} else {
			Logger::logMsg("error loading font");
		}
        
    }
    
    void TextRasterRenderer::renderTTF(const char *text, F32 x, F32 y, F32 scale) {
		while (*text) {
			if (*text >= 32 && *text < 256) {
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, *text-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
                addToBuffer(Vector3f(q.x0, q.y0, 0), Vector3f(q.x1, q.y0, 0), Vector3f(q.x1, q.y1, 0), Vector3f(q.x0, q.y1, 0), Vector2f(q.s0, q.t0), Vector2f(q.s1, q.t0), Vector2f(q.s1, q.t1), Vector2f(q.s0, q.t1));
			}
			++text;
		}
    }
    
    void TextRasterRenderer::renderMultilineTTF(const char *text, F32 x, F32 y, F32 maxX, F32 maxY, F32 scale) {
		S32 i = 0;
		// count the number of actually renderable characters
		F32 origX = x;
		//F32 origY = y;
		F32 lineHeight = getHeight() * vertSpaceMult;
		char c = *text;
		while (c) {
			if (c == '\n') {
				y += lineHeight;
				x = origX;
				if (maxY && y > maxY) {
					break;
				}
			}
			if (c >= 32 && c < 256) {
				if (c == 32) {
                    if (maxX) {
                        // check this word's length and make sure it'll fit.  If not, move to next line.
                        const char *nextSpace = strchr(text+i+1, 32);
                        const char *nextNL = strchr(text+i+1, '\n');
                        const char *wordEnd;
                        if (!nextNL && !nextSpace){
							wordEnd = text+strlen(text);
						} else if (!nextNL) {
                            wordEnd = nextSpace;
                        } else if (!nextSpace) {
                            wordEnd = nextNL;
                        } else {
                        	if (nextSpace < nextNL) {
								wordEnd = nextSpace;
							} else {
								wordEnd = nextNL;
							}
                        }
                        char word[255];
                        S32 wordLength = wordEnd-(text+i+1);
                        if (wordLength > 255) {
                            wordLength = 255;
                        }
                        strncpy(word, text+i+1, wordLength);
                        word[wordLength] = '\0';
                        F32 width = measureTTFWidth(word, scale);
                        if (x + width > maxX) {
                            y += lineHeight;
                            x = origX;
                            ++i;
                            c = *(text + i);
                            if (!c) {
                                break;
                            }
                        }
                    }
 				}
                if (maxY && y > maxY) {
                    break;
                }
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, c-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
                addToBuffer(Vector3f(q.x0, q.y0, 0), Vector3f(q.x1, q.y0, 0), Vector3f(q.x1, q.y1, 0), Vector3f(q.x0, q.y1, 0), Vector2f(q.s0, q.t0), Vector2f(q.s1, q.t0), Vector2f(q.s1, q.t1), Vector2f(q.s0, q.t1));
			}
			++i;
			c = *(text + i);
		}
    }
    
    F32 TextRasterRenderer::getTTFHeight(F32 scale) {
		F32 x = 0;
		F32 y = 0;
		stbtt_aligned_quad q;
		stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, 'A'-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
		// subtract outer stroke amount or it doesn't measure out right
		// Don't scale this value - it's already scaled from GetBakedQuad!
		return q.y1 - q.y0;
    }
    
    F32 TextRasterRenderer::measureTTFWidth(const char *text, F32 scale) {
		F32 x = 0;
		F32 y = 0;
		// high/low diff is the way to go since text often overlaps with non-fixed-width fonts
		F32 lowestX = 999999;
		F32 highestX = 0;
		while (*text) {
			if (*text == '\n') {
				//newline, restart.
				x = 0;
			}
			if (*text >= 32 && *text < 256) {
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, *text-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
				if (q.x0 < lowestX) {
					lowestX = q.x0;
				}
				if (q.x1 > highestX) {
					highestX = q.x1;
				}
			}
			++text;
		}
		// Don't scale this value - it's already scaled from GetBakedQuad!
		return highestX - lowestX;
    }
    
    F32 TextRasterRenderer::measureTTFMultilineHeight(const char *text, F32 availableWidth, F32 scale) {
		S32 i = 0;
		S32 lastSpaceIdx = -1;
		F32 lineHeight = getHeight(scale) * vertSpaceMult;
		F32 x = 0;
		// first line's height
		F32 y = lineHeight;
		char c = *text;
		while (c) {
			if (c == '\n') {
				lastSpaceIdx = -1;
				y += lineHeight;
				x = 0;
			}
			if (c >= 32 && c < 256) {
				if (c == 32) {
					lastSpaceIdx = i;
				}
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, c-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
				if (x > availableWidth) {
					x = 0;
					y += lineHeight;
					// rewind to 1 past last space
					if (lastSpaceIdx != -1) {
						i = lastSpaceIdx + 1;
						lastSpaceIdx = -1;
						c = *(text + i);
					}
					if (c >= 32 && c < 256) {
						stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, c-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
					}
				}
			}
			++i;
			c = *(text + i);
		}
		return y;
    }
    
    static BOOL32 BMGetProperty(char *data, const char *name, char *val) {
        char buf[255];
        strcpy(buf, name);
        strcat(buf, "=");
        char *start = strstr(data, buf);
        if (start) {
            char *valStart = start+strlen(buf);
            char *valEnd;
            if (valStart[0] == '"') {
            	// we're starting with a quote which may have spaces inside so switch to endquote check
            	// we want to include the end quote
            	valEnd = strchr(valStart+1, '"')+1;
            } else {
            	valEnd = strchr(valStart, ' ');
            }
            if (!valEnd) {
                valEnd = strchr(valStart, '\n');
            }
            if (!valEnd) {
                valEnd = strchr(valStart, '\r');
            }
            if (!valEnd) {
                valEnd = strchr(valStart, '\0');
            }
            if (!valEnd) {
                valEnd = valStart;
            }
            S32 len = valEnd-valStart;
            strncpy(val, valStart, len);
            val[len] = '\0';
            return TRUE;
        }
        return FALSE;
    }
    
    static BOOL32 BMGetStringProperty(char *data, const char *name, char **val) {
        char prop[255];
        BOOL32 exists = BMGetProperty(data, name, prop);
        if (exists) {
            char buf[255];
            S32 propLen = strlen(prop);
            strncpy(buf, prop+1, propLen-2);
            buf[propLen-2] = '\0';
            *val = strDuplicate(buf);
            return TRUE;
        }
        return FALSE;
    }
    
    static BOOL32 BMGetIntProperty(char *data, const char *name, S32 *val) {
        char prop[255];
        BOOL32 exists = BMGetProperty(data, name, prop);
        if (exists) {
            *val = atoi(prop);
            return TRUE;
        }
        return FALSE;
    }

    static BOOL32 BMGetU8Property(char *data, const char *name, U8 *val) {
        char prop[255];
        BOOL32 exists = BMGetProperty(data, name, prop);
        if (exists) {
            *val = (U8)atoi(prop);
            return TRUE;
        }
        return FALSE;
    }

    static BOOL32 BMGetBoolProperty(char *data, const char *name, BOOL32 *val) {
        return BMGetIntProperty(data, name, (S32*)val);
    }

    static BOOL32 BMGetFloatProperty(char *data, const char *name, F32 *val) {
        char prop[255];
        BOOL32 exists = BMGetProperty(data, name, prop);
        if (exists) {
            *val = atof(prop);
            return TRUE;
        }
        return FALSE;
    }
    
    static BOOL32 BMGetVector4fProperty(char *data, const char *name, Vector4f *val) {
        char prop[255];
        BOOL32 exists = BMGetProperty(data, name, prop);
        if (exists) {
            char *element = strtok(prop, ",");
            S32 i = 0;
            while (element && i < 4) {
                (*val)[i++] = atof(element);
                element = strtok(NULL, ",");
            }
            return TRUE;
        }
        return FALSE;
    }

    static BOOL32 BMGetVector2fProperty(char *data, const char *name, Vector2f *val) {
        char prop[255];
        BOOL32 exists = BMGetProperty(data, name, prop);
        if (exists) {
            char *element = strtok(prop, ",");
            S32 i = 0;
            while (element && i < 2) {
                (*val)[i++] = atof(element);
                element = strtok(NULL, ",");
            }
            return TRUE;
        }
        
        return FALSE;
    }

    // BMFont
    void TextRasterRenderer::loadBMFont() {
        if (bmInfo) {
            delete bmInfo;
            bmInfo = NULL;
        }
        if (bmFontCharTable) {
            bmFontCharTable->deleteElements();
            delete bmFontCharTable;
            bmFontCharTable = NULL;
        }
        if (bmFontPageTable) {
            bmFontPageTable->deleteElements();
            delete bmFontPageTable;
            bmFontPageTable = NULL;
        }
        if (bmFontPage) {
            delete bmFontPage;
            bmFontPage = NULL;
        }
        char *data = _platform_load_text_asset(assetName);
		if (data) {
			// we'll need a base path of this .fnt file to find the page textures
    		char basePath[512];
    		_platform_convert_path(assetName, basePath);
    		_platform_get_basename(basePath, basePath);
    		// add path separator IE: "/ui/"
    		strcat(basePath, _platform_get_path_separator());
    		// now let's do this in forward-slash so we don't have to do any more converting
    		_platform_convert_path_to_forward(basePath, basePath);
            // parse .fnt file
            bmInfo = new BMFontInfo();
           	S32 kerningsCount = 0;
           	S32 pos = 0;
            char lineBuf[2048];
            BOOL32 more = TextFileUtil::readLine(lineBuf, data, &pos);
            while (more) {
                // parse line
                if (strStartsWith(lineBuf, "info")) {
                    // info line
                    BMGetStringProperty(lineBuf, "face", &bmInfo->face);
                    BMGetFloatProperty(lineBuf, "size", &bmInfo->size);
                    BMGetBoolProperty(lineBuf, "bold", &bmInfo->bold);
                    BMGetBoolProperty(lineBuf, "italic", &bmInfo->italic);
                    BMGetStringProperty(lineBuf, "charset", &bmInfo->charset);
                    BMGetBoolProperty(lineBuf, "unicode", &bmInfo->unicode);
                    BMGetIntProperty(lineBuf, "stretchH", &bmInfo->stretchH);
                    BMGetBoolProperty(lineBuf, "smooth", &bmInfo->smooth);
                    BMGetBoolProperty(lineBuf, "aa", &bmInfo->aa);
                    BMGetVector4fProperty(lineBuf, "padding", &bmInfo->padding);
                    BMGetVector2fProperty(lineBuf, "spacing", &bmInfo->spacing);
                    BMGetFloatProperty(lineBuf, "outline", &bmInfo->outline);
                } else if (strStartsWith(lineBuf, "common")) {
                    // common line
                    BMGetFloatProperty(lineBuf, "lineHeight", &bmInfo->lineHeight);
                    BMGetFloatProperty(lineBuf, "base", &bmInfo->base);
                    BMGetIntProperty(lineBuf, "scaleW", &bmInfo->scaleW);
                    BMGetIntProperty(lineBuf, "scaleH", &bmInfo->scaleH);
                    BMGetIntProperty(lineBuf, "pages", &bmInfo->pages);
                    BMGetBoolProperty(lineBuf, "packed", &bmInfo->packed);
                    BMGetU8Property(lineBuf, "alphaChnl", &bmInfo->alphaChnl);
                    BMGetU8Property(lineBuf, "redChnl", &bmInfo->redChnl);
                    BMGetU8Property(lineBuf, "greenChnl", &bmInfo->greenChnl);
                    BMGetU8Property(lineBuf, "blueChnl", &bmInfo->blueChnl);
                    if (bmInfo->pages > 1) {
                    	bmFontPageTable = new HashTable<S32, BMFontPage*>(bmInfo->pages * 1.5f);
                    } else {
                    	if (bmFontPageTable) {
                    		delete [] bmFontPageTable;
                    		bmFontPageTable = NULL;
                    	}
                    }
                    // prepare for pages
                } else if (strStartsWith(lineBuf, "page")) {
                    // page line
                    // optimize for single page (most common) but hash page ids in case of multiple
                    // push page textures into managed texture pool
                	BMFontPage *page = new BMFontPage();
                	char pageAssetName[512];
    				strcpy(pageAssetName, basePath);
                	BMGetIntProperty(lineBuf, "id", &page->id);
                	char *localFilename;
                	// localFilename will be allocated by BMGetStringProperty
                	BMGetStringProperty(lineBuf, "file", &localFilename);
                	strcat(pageAssetName, localFilename);
                	page->assetName = strDuplicate(pageAssetName);
                	delete [] localFilename;
                    if (bmInfo->pages == 1) {
                    	bmFontPage = page;
                    } else {
                    	bmFontPageTable->put(page->id, page);
                    }
                    if (!context->glResourceManager->getTexture(page->assetName)) {
                        AssetTexture *texture = new AssetTexture(context, page->assetName, FALSE);
                        // default fonts to bilinear because they can look really bad with any nearest neighbor filtering
                        texture->filter = GraphicsConfiguration::BILINEAR;
                        texture->load(TRUE);
                        context->glResourceManager->addTexture(texture);                        
                    };
                } else if (strStartsWith(lineBuf, "chars")) {
                    // chars line
                	if (bmFontCharTable) {
                		delete bmFontCharTable;
                	}
                	S32 count = 0;
                	BMGetIntProperty(lineBuf, "count", &count);
                	bmFontCharTable = new HashTable<S32, BMFontChar*>(count * 1.5f);
                } else if (strStartsWith(lineBuf, "char")) {
                    // char line
                	if (!bmFontCharTable) {
                		logmsg("Error loading BMFont - no char table allocated!");
                	} else {
						BMFontChar *bmChar = new BMFontChar();
						BMGetIntProperty(lineBuf, "id", &bmChar->id);
						BMGetIntProperty(lineBuf, "x", &bmChar->x);
						BMGetIntProperty(lineBuf, "y", &bmChar->y);
						BMGetIntProperty(lineBuf, "width", &bmChar->width);
						BMGetIntProperty(lineBuf, "height", &bmChar->height);
						BMGetIntProperty(lineBuf, "xoffset", &bmChar->xoffset);
						BMGetIntProperty(lineBuf, "yoffset", &bmChar->yoffset);
						BMGetIntProperty(lineBuf, "xadvance", &bmChar->xadvance);
						BMGetIntProperty(lineBuf, "page", &bmChar->page);
						BMGetIntProperty(lineBuf, "chnl", &bmChar->chnl);
						// top left, top right, bottom right, bottom left
						F32 scaleW = bmInfo->scaleW;
						F32 scaleH = bmInfo->scaleH;
						bmChar->uvs[0] = Vector2f((F32)bmChar->x / scaleW, (F32)bmChar->y / scaleH);
						bmChar->uvs[1] = Vector2f((F32)(bmChar->x+bmChar->width) / scaleW, (F32)bmChar->y / scaleH);
						bmChar->uvs[2] = Vector2f((F32)(bmChar->x+bmChar->width) / scaleW, (F32)(bmChar->y+bmChar->height) / scaleH);
						bmChar->uvs[3] = Vector2f((F32)bmChar->x / scaleW, (F32)(bmChar->y+bmChar->height) / scaleH);
						bmFontCharTable->put(bmChar->id, bmChar);
                	}
                } else if (strStartsWith(lineBuf, "kernings")) {
                	// has count
                	if (bmKerningTable) {
                		bmKerningTable->deleteElements();
                		delete bmKerningTable;
                	}
                 	BMGetIntProperty(lineBuf, "count", &kerningsCount);
                	bmKerningTable = new HashTable<S32, HashTable<S32, F32>*>(kerningsCount * 1.5f);
                } else if (strStartsWith(lineBuf, "kerning")) {
                	S32 first, second;
                	F32 amount;
                	BMGetIntProperty(lineBuf, "first", &first);
                	BMGetIntProperty(lineBuf, "second", &second);
                	BMGetFloatProperty(lineBuf, "amount", &amount);
                	HashTable<S32, F32> *kernings = bmKerningTable->get(first);
                	if (!kernings) {
                		kernings = new HashTable<S32, F32>();
                		bmKerningTable->put(first, kernings);
                		kernings->setNullReturnVal(0);
                	}
               		kernings->put(second, amount);
                }
                more = TextFileUtil::readLine(lineBuf, data, &pos);
            }
            _platform_free_asset((unsigned char*)data);
        }
    }
    
    void TextRasterRenderer::renderBMFont(const char *text, F32 x, F32 y, F32 scale) {
    	// adjust scale to requested font size
    	F32 localScale = (fontSize / bmInfo->size) * context->gConfig->uiScale;
    	scale *= localScale;
		// switch to bottom-left to match TTF renderer
		y = y - bmInfo->lineHeight * scale;
		char lastChar = 0;
		while (*text) {
			if (*text >= 32 && *text < 256) {
				BMFontChar *bmChar = bmFontCharTable->get(*text);
				if (bmChar) {
                    if (bmInfo->pages > 1) {
                        if (bmChar->page != bmCurrentPage) {
                            renderBuffer();
                            bmCurrentPage = bmChar->page;
                        }
                    }
                    // now check kerning and adjust
                    F32 kerning = 0;
                    if (bmKerningTable && lastChar) {
                    	HashTable<S32, F32> *kernings = bmKerningTable->get(lastChar);
                    	if (kernings) {
                    		kerning = kernings->get(*text);
                    	}
                    }
                    x += kerning * scale;
					// topleft, topright, bottomright, bottomleft
					Vector3f pos[4];
					F32 offsetx = bmChar->xoffset;
					F32 offsety = bmChar->yoffset;
					Vector3f start(x,y,0);
					pos[0] = Vector3f(offsetx, offsety, 0) * scale + start;
					pos[1] = Vector3f(offsetx+bmChar->width, offsety, 0) * scale + start;
					pos[2] = Vector3f(offsetx+bmChar->width, offsety+bmChar->height, 0) * scale + start;
					pos[3] = Vector3f(offsetx, offsety+bmChar->height, 0) * scale + start;
	                addToBuffer(pos[0], pos[1], pos[2], pos[3], bmChar->uvs[0], bmChar->uvs[1], bmChar->uvs[2], bmChar->uvs[3]);
					x += bmChar->xadvance * scale;
					lastChar = *text;
				}
 			}
			++text;
		}
    }
    
    void TextRasterRenderer::renderMultilineBMFont(const char *text, F32 x, F32 y, F32 maxX, F32 maxY, F32 scale) {
    	// adjust scale to requested font size
    	F32 localScale = (fontSize / bmInfo->size) * context->gConfig->uiScale;
       	scale *= localScale;
		// switch to bottom-left to match TTF renderer
		y = y - bmInfo->lineHeight * scale;
		F32 origX = x;
		//F32 origY = y;
		F32 lineHeight = bmInfo->lineHeight * scale * vertSpaceMult;
		S32 i = 0;
		char lastChar = 0;
		char c = *text;
		while (c) {
			if (c == '\n') {
				y += lineHeight;
				x = origX;
				lastChar = 0;
				if (maxY && y > maxY) {
					break;
				}
			}
			if (c >= 32 && c < 256) {
				if (c == 32) {
					if (maxX) {
                        // check this word's length and make sure it'll fit.  If not, move to next line.
                        const char *nextSpace = strchr(text+i+1, 32);
                        const char *nextNL = strchr(text+i+1, '\n');
                        const char *wordEnd;
                        if (!nextNL && !nextSpace){
							wordEnd = text+strlen(text);
						} else if (!nextNL) {
                            wordEnd = nextSpace;
                        } else if (!nextSpace) {
                            wordEnd = nextNL;
                        } else {
                        	if (nextSpace < nextNL) {
                        		wordEnd = nextSpace;
                        	} else {
                        		wordEnd = nextNL;
                        	}
                         }
                        char word[255];
                        S32 wordLength = wordEnd-(text+i+1);
                        if (wordLength > 255) {
                            wordLength = 255;
                        }
                        strncpy(word, text+i+1, wordLength);
                        word[wordLength] = '\0';
                        // undo the local scale because the measure will reapply
                        F32 width = measureBMFontWidth(word, scale / localScale);
                        if (x + width > maxX) {
                            y += lineHeight;
                            x = origX;
        					lastChar = 0;
                            ++i;
                            c = *(text + i);
                            if (!c) {
                                break;
                            }
                        }
                    }

				}
				BMFontChar *bmChar = bmFontCharTable->get(c);
				if (bmChar) {
                    // now check kerning and adjust
                    F32 kerning = 0;
                    if (bmKerningTable && lastChar) {
                    	HashTable<S32, F32> *kernings = bmKerningTable->get(lastChar);
                    	if (kernings) {
                    		kerning = kernings->get(c);
                    	}
                    }
                    x += kerning * scale;
                    if (bmInfo->pages > 1) {
                        if (bmChar->page != bmCurrentPage) {
                            renderBuffer();
                            bmCurrentPage = bmChar->page;
                        }
                    }
					Vector3f pos[4];
					F32 offsetx = bmChar->xoffset;
					F32 offsety = bmChar->yoffset;
					Vector3f start(x,y,0);
					pos[0] = Vector3f(offsetx, offsety, 0) * scale + start;
					pos[1] = Vector3f(offsetx+bmChar->width, offsety, 0) * scale + start;
					pos[2] = Vector3f(offsetx+bmChar->width, offsety+bmChar->height, 0) * scale + start;
					pos[3] = Vector3f(offsetx, offsety+bmChar->height, 0) * scale + start;
					addToBuffer(pos[0], pos[1], pos[2], pos[3], bmChar->uvs[0], bmChar->uvs[1], bmChar->uvs[2], bmChar->uvs[3]);
					x += bmChar->xadvance * scale;
				}
			}
			++i;
			lastChar = c;
			c = *(text + i);
		}
    }
    
    F32 TextRasterRenderer::getBMFontHeight(F32 scale) {
    	// adjust scale to requested font size
    	F32 localScale = (fontSize / bmInfo->size) * context->gConfig->uiScale;
    	scale *= localScale;
        return bmInfo->lineHeight * scale;
    }
    
    F32 TextRasterRenderer::measureBMFontWidth(const char *text, F32 scale) {
    	// adjust scale to requested font size
    	F32 localScale = (fontSize / bmInfo->size) * context->gConfig->uiScale;
    	scale *= localScale;
		F32 width = 0;
		F32 longestWidth = 0;
		char lastChar = 0;
		while (*text) {
			if (*text == '\n') {
				//newline, restart.
				if (width > longestWidth) {
					longestWidth = width;
				}
				width = 0;
			}
			if (*text >= 32 && *text < 256) {
				BMFontChar *bmChar = bmFontCharTable->get(*text);
				if (bmChar) {
                    // now check kerning and adjust
                    F32 kerning = 0;
                    if (bmKerningTable && lastChar) {
                    	HashTable<S32, F32> *kernings = bmKerningTable->get(lastChar);
                    	if (kernings) {
                    		kerning = kernings->get(*text);
                    	}
                    }
 					width += bmChar->xadvance;
                    width += kerning;
				}
			}
			lastChar = *text;
			++text;
		}
		if (width > longestWidth) {
			longestWidth = width;
		}
		return longestWidth * scale;

    }
    
    F32 TextRasterRenderer::measureBMFontMultilineHeight(const char *text, F32 availableWidth, F32 scale) {
    	// adjust scale to requested font size
    	F32 localScale = (fontSize / bmInfo->size) * context->gConfig->uiScale;
    	scale *= localScale;
		S32 i = 0;
		S32 lastSpaceIdx = -1;
		F32 lineHeight = getHeight(scale) * vertSpaceMult;
		F32 x = 0;
		// first line's height
		F32 y = lineHeight;
		char c = *text;
		char lastChar = 0;
		while (c) {
			if (c == '\n') {
				lastSpaceIdx = -1;
				y += lineHeight;
				x = 0;
			}
			if (c >= 32 && c < 256) {
                // now check kerning and adjust
                F32 kerning = 0;
                if (x > 0 && bmKerningTable && lastChar) {
                	HashTable<S32, F32> *kernings = bmKerningTable->get(lastChar);
                	if (kernings) {
                		kerning = kernings->get(*text);
                	}
                }
				if (c == 32) {
					lastSpaceIdx = i;
				}
				BMFontChar *bmChar = bmFontCharTable->get(*text);
				if (bmChar) {
					x += bmChar->xadvance + kerning;
				}
				if (x > availableWidth) {
					x = 0;
					y += lineHeight;
					lastChar = 0;
					// rewind to 1 past last space
					if (lastSpaceIdx != -1) {
						i = lastSpaceIdx + 1;
						lastSpaceIdx = -1;
						c = *(text + i);
					}
					if (c >= 32 && c < 256) {
						bmChar = bmFontCharTable->get(*text);
						if (bmChar) {
							x += bmChar->xadvance;
						}
					}
				}
			}
			++i;
			lastChar = c;
			c = *(text + i);
		}
		return y;
    }

	void TextRasterRenderer::init(BOOL32 newContext) {
        if (!newContext) {
            return;
        }
        if (strEndsWith(assetName, "fnt") || strEndsWith(assetName, "FNT")) {
            isTTF = FALSE;
        } else {
            isTTF = TRUE;
        }
        // dirty - shouldn't this be per-font?
		vertSpaceMult = context->appProperties->get("text_vertical_spacing_multiplier")->getFloatValue();
        if (isTTF) {
            loadTTF();
        } else {
            loadBMFont();
        }
        if (USE_VBOS_WHEN_AVAILBLE && (context->gConfig->useShaders || context->gConfig->supportsVBOs)) {
            // init VBO support
            GLuint bufferIDs[2];
            glGenBuffers(2, bufferIDs);
            vertVBOId = bufferIDs[0];
            idxVBOId = bufferIDs[1];
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxVBOId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, TEXT_RENDER_BUFFER_SIZE * 6 * sizeof(U16), idxBuffer, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, vertVBOId);
            glBufferData(GL_ARRAY_BUFFER, TEXT_RENDER_BUFFER_SIZE * 4 * sizeof(GLQuadVertex), vertBuffer, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
        }
	}

	F32 TextRasterRenderer::getHeight(F32 scale) {
        if (isTTF) {
            return getTTFHeight(scale);
        } else {
            return getBMFontHeight(scale);
        }
	}

	F32 TextRasterRenderer::measureWidth(const char *text, F32 scale) {
        if (isTTF) {
            return measureTTFWidth(text, scale);
        } else {
            return measureBMFontWidth(text, scale);
        }
	}

	F32 TextRasterRenderer::measureMultilineHeight(const char *text, F32 availableWidth, F32 scale) {
        if (isTTF) {
            return measureTTFMultilineHeight(text, availableWidth, scale);
        } else {
            return measureBMFontMultilineHeight(text, availableWidth, scale);
        }
	}
    
	void TextRasterRenderer::render(const char *text, F32 x, F32 y, F32 scale) {
        if (isTTF) {
            renderTTF(text, x, y, scale);
        } else {
            renderBMFont(text, x, y, scale);
        }
	}
    
	void TextRasterRenderer::renderMultiline(const char *text, F32 x, F32 y, F32 maxX, F32 maxY, F32 scale) {
        if (isTTF) {
            renderMultilineTTF(text, x, y, maxX, maxY, scale);
        } else {
            renderMultilineBMFont(text, x, y, maxX, maxY, scale);
        }
	}
    
	void TextRasterRenderer::startText() {
		if (ftex) {
			Texture::textureSwitches++;
			glBindTexture(GL_TEXTURE_2D, ftex);
			Texture::lastTextureId = ftex;
		}
		if (context->gConfig->useShaders) {
			ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram(SHADER_PROGRAM_TAG);
			if (shaderProgram) {
				shaderProgram->bind();
			}
		}
		context->renderContext->colorFilter = color;
        if (!USE_VBOS_WHEN_AVAILBLE && (context->gConfig->useShaders || context->gConfig->supportsVBOs)) {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
	}

	void TextRasterRenderer::finishText() {
		//glDisable(GL_BLEND);
        renderBuffer();
		if (context->gConfig->useShaders) {
			ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram(SHADER_PROGRAM_TAG);
			if (shaderProgram) {
				shaderProgram->unbind();
			}
		}
	}

    void TextRasterRenderer::addToBuffer(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Vector2f uv0, Vector2f uv1, Vector2f uv2, Vector2f uv3) {
        if (bufferUsed >= TEXT_RENDER_BUFFER_SIZE) {
            // flush the buffer by drawing and keep going
            renderBuffer();
        }
        vertBuffer[bufferUsed*4].position = p0;
        vertBuffer[bufferUsed*4+1].position = p1;
        vertBuffer[bufferUsed*4+2].position = p2;
        vertBuffer[bufferUsed*4+3].position = p3;
        vertBuffer[bufferUsed*4].uv = uv0;
        vertBuffer[bufferUsed*4+1].uv = uv1;
        vertBuffer[bufferUsed*4+2].uv = uv2;
        vertBuffer[bufferUsed*4+3].uv = uv3;
        bufferUsed++;
    }

    void TextRasterRenderer::renderBuffer() {
        if (bufferUsed == 0) {
            return;
        }
        if (!isTTF) {
        	if (bmInfo->pages == 1) {
        		Texture *tex = context->glResourceManager->getTexture(bmFontPage->assetName);
        		if (tex) {
        			tex->bind();
        		}
        	} else {
                BMFontPage *page = bmFontPageTable->get(bmCurrentPage);
                if (page) {
                    Texture *tex = context->glResourceManager->getTexture(page->assetName);
                    if (tex) {
                        tex->bind();
                    }
                }
            }
        }
        BOOL32 useVBO = FALSE;
        if (USE_VBOS_WHEN_AVAILBLE && (context->gConfig->useShaders || context->gConfig->supportsVBOs)) {
            glBindBuffer(GL_ARRAY_BUFFER, vertVBOId);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxVBOId);
            glBufferSubData(GL_ARRAY_BUFFER, 0, bufferUsed*sizeof(GLQuadVertex)*4, vertBuffer);
            useVBO = TRUE;
        }
		if (context->gConfig->useShaders) {
			ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram(SHADER_PROGRAM_TAG);
			if (shaderProgram) {
                if (useVBO) {
                    glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), BUFFER_OFFSET(0));
                    glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), BUFFER_OFFSET(sizeof(Vector3f)));
                    glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vColor"), 4, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), BUFFER_OFFSET(sizeof(Vector3f)+sizeof(Vector2f)));
                 } else {
                     glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), &vertBuffer[0].position);
                     glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), &vertBuffer[0].uv);
                     glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vColor"), 4, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), &vertBuffer[0].color);
                 }                   
                glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
                glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->mvMatrix.data);
                glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
                Vector4f colorFilter = context->renderContext->colorFilter;
                glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
			}
		} else {
			Vector4f colorFilter = context->renderContext->colorFilter;
			glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
            if (useVBO) {
                glVertexPointer(3, GL_FLOAT, sizeof(GLQuadVertex), BUFFER_OFFSET(0));
                glTexCoordPointer(2, GL_FLOAT, sizeof(GLQuadVertex), BUFFER_OFFSET(sizeof(Vector3f)));
                glColorPointer(4, GL_FLOAT, sizeof(GLQuadVertex), BUFFER_OFFSET(sizeof(Vector3f)+sizeof(Vector2f)));
           } else {
                glVertexPointer(3, GL_FLOAT, sizeof(GLQuadVertex), &vertBuffer[0].position);
                glTexCoordPointer(2, GL_FLOAT, sizeof(GLQuadVertex), &vertBuffer[0].uv);
                glColorPointer(4, GL_FLOAT, sizeof(GLQuadVertex), &vertBuffer[0].color);
            }
		}
        if (useVBO) {
            glDrawElements(GL_TRIANGLES, bufferUsed*6, GL_UNSIGNED_SHORT, 0);
        } else {
            glDrawElements(GL_TRIANGLES, bufferUsed*6, GL_UNSIGNED_SHORT, idxBuffer);
        }
        bufferUsed = 0;
    }
    
	void TextRasterRenderer::applyOuterStroke(unsigned char* bitmap8, unsigned char* bitmap, S32 width, S32 height) {
		S32 strokeAmt = this->outerStroke;
		for (S32 y = 0; y < height; y++) {
			for (S32 x = 0; x < width; x++) {
				unsigned char color = bitmap8[y * width + x];
				if (!color) {
					BOOL32 hasAdjacentColor = FALSE;
					// search adjacent for color
					for (S32 y2 = y - strokeAmt; y2 <= y + strokeAmt; y2++) {
						for (S32 x2 = x - strokeAmt; x2 <= x + strokeAmt; x2++) {
							if (x2 < 0 || y2 < 0 || x2 >= width || y2 >= height) {
								continue;
							}
							unsigned char color2 = bitmap8[y2 * width + x2];
							if (color2) {
								hasAdjacentColor = TRUE;
								break;
							}
						}
						if (hasAdjacentColor) {
							break;
						}
					}
					if (hasAdjacentColor) {
						bitmap[(y * width + x) * 4] = 0;
						bitmap[(y * width + x) * 4 + 1] = 0;
						bitmap[(y * width + x) * 4 + 2] = 0;
						bitmap[(y * width + x) * 4 + 3] = 0xFF;
					}
				}
			}
		}
	}

	void TextRasterRenderer::applyInnerStroke(unsigned char* bitmap8, unsigned char* bitmap, S32 width, S32 height) {
		S32 strokeAmt = this->innerStroke;
		for (S32 y = 0; y < height; y++) {
			for (S32 x = 0; x < width; x++) {
				unsigned char color = bitmap8[y * width + x];
				if (color) {
					BOOL32 hasAdjacentAlpha = FALSE;
					// search adjacent for alpha
					for (S32 y2 = y - strokeAmt; y2 <= y + strokeAmt; y2++) {
						for (S32 x2 = x - strokeAmt; x2 <= x + strokeAmt; x2++) {
							if (x2 < 0 || y2 < 0 || x2 >= width || y2 >= height) {
								continue;
							}
							unsigned char color2 = bitmap8[y2 * width + x2];
							if (!color2) {
								hasAdjacentAlpha = TRUE;
								break;
							}
						}
						if (hasAdjacentAlpha) {
							break;
						}
					}
					if (hasAdjacentAlpha) {
						bitmap[(y * width + x) * 4] = 0;
						bitmap[(y * width + x) * 4 + 1] = 0;
						bitmap[(y * width + x) * 4 + 2] = 0;
						bitmap[(y * width + x) * 4 + 3] = 0xFF;
					}
				}
			}
		}
	}

    void TextRasterRenderer::applyAA(unsigned char* bitmap, S32 width, S32 height) {
        S32 rowBytes = width * 4;
		for (S32 y = 0; y < height; y++) {
            S32 row = y * width;
			for (S32 x = 0; x < width; x++) {
                S32 pixelAddr = (row + x) * 4;
                S32 r = 0, b = 0, g = 0, a = 0;
                S32 count = 0;
                // above
                if (y > 0) {
                    r += bitmap[pixelAddr-rowBytes];
                    g += bitmap[pixelAddr+1-rowBytes];
                    b += bitmap[pixelAddr+2-rowBytes];
                    a += bitmap[pixelAddr+3-rowBytes];
                    count++;
                }
                // left
                if (x > 0) {
                    r += bitmap[pixelAddr-4];
                    g += bitmap[pixelAddr+1-4];
                    b += bitmap[pixelAddr+2-4];
                    a += bitmap[pixelAddr+3-4];
                    count++;
                }
                // below
                if (y < height-1) {
                    r += bitmap[pixelAddr+rowBytes];
                    g += bitmap[pixelAddr+1+rowBytes];
                    b += bitmap[pixelAddr+2+rowBytes];
                    a += bitmap[pixelAddr+3+rowBytes];
                    count++;
                }
                // right
                if (x < width-1) {
                    r += bitmap[pixelAddr+4];
                    g += bitmap[pixelAddr+1+4];
                    b += bitmap[pixelAddr+2+4];
                    a += bitmap[pixelAddr+3+4];
                    count++;
                }
                bitmap[pixelAddr] = (bitmap[pixelAddr] + r/count)/2;
                bitmap[pixelAddr + 1] = (bitmap[pixelAddr+1] + g/count)/2;
                bitmap[pixelAddr + 2] = (bitmap[pixelAddr+2] + b/count)/2;
                bitmap[pixelAddr + 3] = (bitmap[pixelAddr+3] + a/count)/2;;
            }
        }
    }
}
