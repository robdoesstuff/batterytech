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

// uncomment this if there are strange text artifacts - it will put debuggable values into the text arrays which
// will show any algorithm errors in layout if any array positions are skipped accidentally.
// #define DEBUG_TEXT_ELEMENT_ARRAY

#define SHADER_PROGRAM_TAG "quad"

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))

namespace BatteryTech {

	TextRasterRenderer::TextRasterRenderer(Context *context, const char *assetName, F32 fontSize) {
		this->context = context;
		aName = strDuplicate(assetName);
		this->fontSize = fontSize;
		bmpWidth = 0;
		bmpHeight = 0;
		innerStroke = 0;
		outerStroke = 0;
		color = Vector4f(1,1,1,1);
        vertVBOId = 0;
        idxVBOId = 0;
        vertBuffer = new GLQuadVertex[TEXT_RENDER_MAX_MULTILINE_LENGTH * 4];
        idxBuffer = new U16[TEXT_RENDER_MAX_MULTILINE_LENGTH*6];
	}

	TextRasterRenderer::~TextRasterRenderer() {
		delete [] aName;
        delete [] vertBuffer;
        delete [] idxBuffer;
	}

	void TextRasterRenderer::setStroke(S32 inner, S32 outer) {
		innerStroke = inner * context->gConfig->uiScale;
		outerStroke = outer * context->gConfig->uiScale;
	}

	void TextRasterRenderer::setColorFilter(Vector4f rgba) {
		color = rgba;
	}

	void TextRasterRenderer::init(BOOL32 newContext) {
        if (!newContext) {
            return;
        }
		S32 bmpWidth = context->appProperties->get("initial_font_texture_width")->getIntValue();
		S32 bmpHeight = context->appProperties->get("initial_font_texture_height")->getIntValue();
		vertSpaceMult = context->appProperties->get("text_vertical_spacing_multiplier")->getFloatValue();
		S32 size = 0;
		BYTE8 *data;
		data = _platform_load_asset(aName, &size);
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
			printf("Font Metrics: ascent=%d, descent=%d, lineGap=%d, awm=%d, mlsb=%d, mrsb=%d, xmax=%d\n", ascent, descent, lineGap, advanceWidthMax, minLeftSideBearing, minRightSideBearing, xMaxExtent);
			F32 scaledFontSize = fontSize * context->gConfig->uiScale;
			F32 scale = stbtt_ScaleForPixelHeight(&f, scaledFontSize);
			printf("Bitmap Scale: %f = %f, awmScaled=%f, xmaxScaled=%f\n", scaledFontSize, scale, advanceWidthMax*scale, xMaxExtent*scale);
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
			GLenum error = glGetError();
			if (error) {
				Logger::logMsg("GL Error");
			}
			// can free temp_bitmap at this point
			_platform_free_asset(data);
            // generate the face indices
            for (S32 i = 0; i < TEXT_RENDER_MAX_MULTILINE_LENGTH; i++) {
                S32 ii = i*6;
                S32 jj = i*4;
                idxBuffer[ii+0] = jj+0;
                idxBuffer[ii+1] = jj+1;
                idxBuffer[ii+2] = jj+2;
                idxBuffer[ii+3] = jj+0;
                idxBuffer[ii+4] = jj+2;
                idxBuffer[ii+5] = jj+3;
            }
            if (context->gConfig->supportsShaders || context->gConfig->supportsVBOs) {
                // init VBO support
                GLuint bufferIDs[2];
                glGenBuffers(2, bufferIDs);
                vertVBOId = bufferIDs[0];
                idxVBOId = bufferIDs[1];
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxVBOId);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, TEXT_RENDER_MAX_MULTILINE_LENGTH * 6 * sizeof(U16), idxBuffer, GL_STATIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ARRAY_BUFFER, vertVBOId);
                glBufferData(GL_ARRAY_BUFFER, TEXT_RENDER_MAX_MULTILINE_LENGTH * 4 * sizeof(GLQuadVertex), vertBuffer, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

            }
		} else {
			Logger::logMsg("error loading font");
		}
	}

	F32 TextRasterRenderer::getHeight(F32 scale) {
		F32 x = 0;
		F32 y = 0;
		stbtt_aligned_quad q;
		stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, 'A'-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
		// subtract outer stroke amount or it doesn't measure out right
		return (q.y1 - q.y0) * scale;
	}

	F32 TextRasterRenderer::measureWidth(const char *text, F32 scale) {
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
		return (highestX - lowestX) * scale;
	}

	void TextRasterRenderer::startText() {
		Texture::textureSwitches++;
		glBindTexture(GL_TEXTURE_2D, ftex);
		Texture::lastTextureId = ftex;
		if (context->gConfig->useShaders) {
			ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram(SHADER_PROGRAM_TAG);
			if (shaderProgram) {
				shaderProgram->bind();
			}
		}
		context->renderContext->colorFilter = color;
	}

	void TextRasterRenderer::finishText() {
		//glDisable(GL_BLEND);
		if (context->gConfig->useShaders) {
			ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram(SHADER_PROGRAM_TAG);
			if (shaderProgram) {
				shaderProgram->unbind();
			}
		}
	}

	void TextRasterRenderer::render(const char *text, F32 x, F32 y, F32 scale) {
		S32 length = strlen(text);
		// longest string is TEXT_RENDER_MAX_LINE_LENGTH
		S32 unicodes[TEXT_RENDER_MAX_LINE_LENGTH];
		length = strnUTF8ToUnicodeArray(text, unicodes, TEXT_RENDER_MAX_LINE_LENGTH);
		if (length > TEXT_RENDER_MAX_LINE_LENGTH) {
			length = TEXT_RENDER_MAX_LINE_LENGTH;
		}
		S32 i = 0;
		// count the number of actually renderable characters
		S32 renderChars = 0;
		while (*text && i <= TEXT_RENDER_MAX_LINE_LENGTH) {
			if (*text >= 32 && *text < 256) {
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, *text-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
                vertBuffer[renderChars*4].position = Vector3f(q.x0, q.y0, 0);
                vertBuffer[renderChars*4+1].position = Vector3f(q.x1, q.y0, 0);
                vertBuffer[renderChars*4+2].position = Vector3f(q.x1, q.y1, 0);
                vertBuffer[renderChars*4+3].position = Vector3f(q.x0, q.y1, 0);
                vertBuffer[renderChars*4].uv = Vector2f(q.s0, q.t0);
                vertBuffer[renderChars*4+1].uv = Vector2f(q.s1, q.t0);
                vertBuffer[renderChars*4+2].uv = Vector2f(q.s1, q.t1);
                vertBuffer[renderChars*4+3].uv = Vector2f(q.s0, q.t1);
                ++renderChars;
			}
            ++i;
			++text;
		}
        length = i;
        if (length == 0) {
            return;
        }
        renderBuffer(renderChars);
	}

    void TextRasterRenderer::renderBuffer(S32 count) {
        BOOL32 useVBO = FALSE;
        if (context->gConfig->useShaders || context->gConfig->supportsVBOs) {
            glBindBuffer(GL_ARRAY_BUFFER, vertVBOId);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxVBOId);
            glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(GLQuadVertex)*4, vertBuffer);
            useVBO = TRUE;
        }
		if (context->gConfig->useShaders) {
			ShaderProgram *shaderProgram = context->glResourceManager->getShaderProgram(SHADER_PROGRAM_TAG);
			if (shaderProgram) {
				glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), BUFFER_OFFSET(0));
				glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), BUFFER_OFFSET(sizeof(Vector3f)));
                glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vColor"), 4, GL_FLOAT, GL_FALSE, sizeof(GLQuadVertex), BUFFER_OFFSET(sizeof(Vector3f)+sizeof(Vector2f)));
				glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
				glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->mvMatrix.data);
				glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
				Vector4f colorFilter = context->renderContext->colorFilter;
				glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
			}
		} else {
			Vector4f colorFilter = context->renderContext->colorFilter;
			glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
            if (context->gConfig->supportsVBOs) {
                glVertexPointer(3, GL_FLOAT, sizeof(GLQuadVertex), BUFFER_OFFSET(0));
                glTexCoordPointer(2, GL_FLOAT, sizeof(GLQuadVertex), BUFFER_OFFSET(sizeof(Vector3f)));
            } else {
                glVertexPointer(3, GL_FLOAT, sizeof(GLQuadVertex), vertBuffer);
                glTexCoordPointer(2, GL_FLOAT, sizeof(GLQuadVertex), vertBuffer+sizeof(Vector3f));
            }
		}
        if (useVBO) {
            glDrawElements(GL_TRIANGLES, count*6, GL_UNSIGNED_SHORT, 0);
        } else {
            glDrawElements(GL_TRIANGLES, count*6, GL_UNSIGNED_SHORT, idxBuffer);
        }
        
    }
    
	F32 TextRasterRenderer::measureMultilineHeight(const char *text, F32 availableWidth, F32 scale) {
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
					if (c >= 32 && c < 128) {
						stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, c-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
					}
				}
			}
			++i;
			c = *(text + i);
		}
		return y;
	}

	void TextRasterRenderer::renderMultiline(const char *text, F32 x, F32 y, F32 maxX, F32 maxY, F32 scale) {
		S32 length = strlen(text);
		if (length > TEXT_RENDER_MAX_MULTILINE_LENGTH) {
			length = TEXT_RENDER_MAX_MULTILINE_LENGTH;
		}
		// longest string is TEXT_RENDER_MAX_MULTILINE_LENGTH
		S32 i = 0;
		// count the number of actually renderable characters
		S32 renderChars = 0;
		S32 lastSpaceIdx = -1;
		S32 lastSpaceRenderIdx = -1;
		F32 origX = x;
		//F32 origY = y;
		F32 lineHeight = getHeight() * vertSpaceMult;
		char c = *text;
		while (c && i <= TEXT_RENDER_MAX_MULTILINE_LENGTH) {
			if (c == '\n') {
				lastSpaceIdx = -1;
				lastSpaceRenderIdx = -1;
				y += lineHeight;
				x = origX;
				if (y > maxY) {
					break;
				}
			}
			if (c >= 32 && c < 256) {
				if (c == 32) {
					lastSpaceIdx = i;
					lastSpaceRenderIdx = renderChars;
				}
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, c-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
				if (x > maxX) {
					x = origX;
					y += lineHeight;
					// rewind to char index after last space (but last space render char)
					if (lastSpaceIdx != -1) {
						i = lastSpaceIdx + 1;
						renderChars = lastSpaceRenderIdx;
						lastSpaceIdx = -1;
						lastSpaceRenderIdx = -1;
						c = *(text + i);
					}
					if (y > maxY) {
						break;
					} else {
						if (c >= 32 && c < 256) {
							stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, c-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
						}
					}
				}
                vertBuffer[renderChars*4].position = Vector3f(q.x0, q.y0, 0);
                vertBuffer[renderChars*4+1].position = Vector3f(q.x1, q.y0, 0);
                vertBuffer[renderChars*4+2].position = Vector3f(q.x1, q.y1, 0);
                vertBuffer[renderChars*4+3].position = Vector3f(q.x0, q.y1, 0);
                vertBuffer[renderChars*4].uv = Vector2f(q.s0, q.t0);
                vertBuffer[renderChars*4+1].uv = Vector2f(q.s1, q.t0);
                vertBuffer[renderChars*4+2].uv = Vector2f(q.s1, q.t1);
                vertBuffer[renderChars*4+3].uv = Vector2f(q.s0, q.t1);
				++renderChars;
			}
			++i;
			c = *(text + i);
		}
		renderBuffer(renderChars);
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
