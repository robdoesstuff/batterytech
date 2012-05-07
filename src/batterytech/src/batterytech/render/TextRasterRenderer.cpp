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

// uncomment this if there are strange text artifacts - it will put debuggable values into the text arrays which
// will show any algorithm errors in layout if any array positions are skipped accidentally.
// #define DEBUG_TEXT_ELEMENT_ARRAY

namespace BatteryTech {

	TextRasterRenderer::TextRasterRenderer(Context *context, const char *assetName, F32 fontSize) {
		this->context = context;
		aName = strDuplicate(assetName);
		this->fontSize = fontSize;
		bmpWidth = 0;
		bmpHeight = 0;
		shaderProgram = new ShaderProgram("shaders/quadshader.vert", "shaders/quadshader.frag");
		innerStroke = 0;
		outerStroke = 0;
		color = Vector4f(1,1,1,1);
	}

	TextRasterRenderer::~TextRasterRenderer() {
		delete shaderProgram;
		delete [] aName;
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
		} else {
			Logger::logMsg("error loading font");
		}
		if (context->gConfig->useShaders) {
			shaderProgram->init(newContext);
			shaderProgram->addVertexAttributeLoc("vPosition");
			shaderProgram->addVertexAttributeLoc("uvMap");
			shaderProgram->addUniformLoc("projection_matrix");
			shaderProgram->addUniformLoc("modelview_matrix");
			shaderProgram->addUniformLoc("tex");
			shaderProgram->addUniformLoc("colorFilter");
		}
	}

	F32 TextRasterRenderer::getHeight(F32 scale) {
		F32 x = 0;
		F32 y = 0;
		stbtt_aligned_quad q;
		stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, 'A'-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
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
			shaderProgram->bind();
		}
		context->renderContext->colorFilter = color;
	}

	void TextRasterRenderer::finishText() {
		//glDisable(GL_BLEND);
		if (context->gConfig->useShaders) {
			shaderProgram->unbind();
		}
	}

	void TextRasterRenderer::render(const char *text, F32 x, F32 y, F32 scale) {
		S32 length = strlen(text);
		// longest string is TEXT_RENDER_MAX_LINE_LENGTH
		F32 verts[TEXT_RENDER_MAX_LINE_LENGTH * 6 * 3];
		F32 uvs[TEXT_RENDER_MAX_LINE_LENGTH * 6 * 2];
		S32 unicodes[TEXT_RENDER_MAX_LINE_LENGTH];
		length = strnUTF8ToUnicodeArray(text, unicodes, TEXT_RENDER_MAX_LINE_LENGTH);
		if (length > TEXT_RENDER_MAX_LINE_LENGTH) {
			length = TEXT_RENDER_MAX_LINE_LENGTH;
		}
		S32 i = 0;
#ifdef DEBUG_TEXT_ELEMENT_ARRAY
		for (S32 i = 0; i < TEXT_RENDER_MAX_LINE_LENGTH; i++) {
			// top left
			verts[i * 18] = verts[i * 18 + 9] = 0;
			verts[i * 18 + 1] = verts[i * 18 + 9 + 10] = 0;
			verts[i * 18 + 2] = verts[i * 18 + 9 + 11] = 0;
			// top right
			verts[i * 18 + 3] = context->gConfig->viewportWidth;
			verts[i * 18 + 4] = 0;
			verts[i * 18 + 5] = 0;
			// bottom right
			verts[i * 18 + 6] = verts[i * 18 + 12] = context->gConfig->viewportWidth;
			verts[i * 18 + 7] = verts[i * 18 + 13] = context->gConfig->viewportHeight;
			verts[i * 18 + 8] = verts[i * 18 + 14] = 0;
			// bottom left
			verts[i * 18 + 15] = 0;
			verts[i * 18 + 16] = context->gConfig->viewportHeight;
			verts[i * 18 + 17] = 0;
			// 0
			uvs[i * 12] = uvs[i * 12 + 6] = 0;
			uvs[i * 12 + 1] = uvs[i * 12 + 7] = 0;
			// 1
			uvs[i * 12 + 2] = 1.0;
			uvs[i * 12 + 3] = 0;
			// 2
			uvs[i * 12 + 4] = uvs[i * 12 + 8] = 1.0;
			uvs[i * 12 + 5] = uvs[i * 12 + 9] = 1.0;
			// 3
			uvs[i * 12 + 10] = 0;
			uvs[i * 12 + 11] = 1.0;
		}
#endif
		while (*text && i <= TEXT_RENDER_MAX_LINE_LENGTH) {
			if (*text >= 32 && *text < 256) {
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, *text-32, &x,&y,&q,1, scale);//1=opengl,0=old d3d
				S32 pos = i * 18;
				// 0
				verts[pos] = q.x0;
				verts[pos + 1] = q.y0;
				verts[pos + 2] = 0;
				// 1
				verts[pos + 3] = q.x1;
				verts[pos + 4] = q.y0;
				verts[pos + 5] = 0;
				// 2
				verts[pos + 6] = q.x1;
				verts[pos + 7] = q.y1;
				verts[pos + 8] = 0;
				// 0
				verts[pos + 9] = q.x0;
				verts[pos + 10] = q.y0;
				verts[pos + 11] = 0;
				// 2
				verts[pos + 12] = q.x1;
				verts[pos + 13] = q.y1;
				verts[pos + 14] = 0;
				// 3
				verts[pos + 15] = q.x0;
				verts[pos + 16] = q.y1;
				verts[pos + 17] = 0;
				pos = i * 12;
				// 0
				uvs[pos] = q.s0;
				uvs[pos + 1] = q.t0;
				// 1
				uvs[pos + 2] = q.s1;
				uvs[pos + 3] = q.t0;
				// 2
				uvs[pos + 4] = q.s1;
				uvs[pos + 5] = q.t1;
				// 0
				uvs[pos + 6] = q.s0;
				uvs[pos + 7] = q.t0;
				// 2
				uvs[pos + 8] = q.s1;
				uvs[pos + 9] = q.t1;
				// 3
				uvs[pos + 10] = q.s0;
				uvs[pos + 11] = q.t1;
			}
			++text;
			++i;
		}
		if (context->gConfig->useShaders) {
			glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, verts);
			glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, 0, uvs);
			glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
			glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->mvMatrix.data);
			glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
			Vector4f colorFilter = context->renderContext->colorFilter;
			glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		} else {
			Vector4f colorFilter = context->renderContext->colorFilter;
			glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
			glVertexPointer(3, GL_FLOAT, 0, &verts);
			glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		}
		if (length > 0) {
			glDrawArrays(GL_TRIANGLES, 0, length * 6);
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
		F32 verts[TEXT_RENDER_MAX_MULTILINE_LENGTH * 6 * 3];
		F32 uvs[TEXT_RENDER_MAX_MULTILINE_LENGTH * 6 * 2];
#ifdef DEBUG_TEXT_ELEMENT_ARRAY
		for (S32 i = 0; i < TEXT_RENDER_MAX_MULTILINE_LENGTH; i++) {
			// top left
			verts[i * 18] = verts[i * 18 + 9] = 0;
			verts[i * 18 + 1] = verts[i * 18 + 9 + 10] = 0;
			verts[i * 18 + 2] = verts[i * 18 + 9 + 11] = 0;
			// top right
			verts[i * 18 + 3] = context->gConfig->viewportWidth;
			verts[i * 18 + 4] = 0;
			verts[i * 18 + 5] = 0;
			// bottom right
			verts[i * 18 + 6] = verts[i * 18 + 12] = context->gConfig->viewportWidth;
			verts[i * 18 + 7] = verts[i * 18 + 13] = context->gConfig->viewportHeight;
			verts[i * 18 + 8] = verts[i * 18 + 14] = 0;
			// bottom left
			verts[i * 18 + 15] = 0;
			verts[i * 18 + 16] = context->gConfig->viewportHeight;
			verts[i * 18 + 17] = 0;
			// 0
			uvs[i * 12] = uvs[i * 12 + 6] = 0;
			uvs[i * 12 + 1] = uvs[i * 12 + 7] = 0;
			// 1
			uvs[i * 12 + 2] = 1.0;
			uvs[i * 12 + 3] = 0;
			// 2
			uvs[i * 12 + 4] = uvs[i * 12 + 8] = 1.0;
			uvs[i * 12 + 5] = uvs[i * 12 + 9] = 1.0;
			// 3
			uvs[i * 12 + 10] = 0;
			uvs[i * 12 + 11] = 1.0;
		}
#endif
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
				S32 pos = renderChars * 18;
				// 0
				verts[pos] = q.x0;
				verts[pos + 1] = q.y0;
				verts[pos + 2] = 0;
				// 1
				verts[pos + 3] = q.x1;
				verts[pos + 4] = q.y0;
				verts[pos + 5] = 0;
				// 2
				verts[pos + 6] = q.x1;
				verts[pos + 7] = q.y1;
				verts[pos + 8] = 0;
				// 0
				verts[pos + 9] = q.x0;
				verts[pos + 10] = q.y0;
				verts[pos + 11] = 0;
				// 2
				verts[pos + 12] = q.x1;
				verts[pos + 13] = q.y1;
				verts[pos + 14] = 0;
				// 3
				verts[pos + 15] = q.x0;
				verts[pos + 16] = q.y1;
				verts[pos + 17] = 0;
				pos = renderChars * 12;
				// 0
				uvs[pos] = q.s0;
				uvs[pos + 1] = q.t0;
				// 1
				uvs[pos + 2] = q.s1;
				uvs[pos + 3] = q.t0;
				// 2
				uvs[pos + 4] = q.s1;
				uvs[pos + 5] = q.t1;
				// 0
				uvs[pos + 6] = q.s0;
				uvs[pos + 7] = q.t0;
				// 2
				uvs[pos + 8] = q.s1;
				uvs[pos + 9] = q.t1;
				// 3
				uvs[pos + 10] = q.s0;
				uvs[pos + 11] = q.t1;
				//char buf[100];
				//sprintf(buf, "RenderChar %d (%c) is pos %d", renderChars, c, pos);
				//logmsg(buf);
				++renderChars;
			}
			++i;
			c = *(text + i);
		}
		if (context->gConfig->useShaders) {
			glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, verts);
			glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, 0, uvs);
			glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
			glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->mvMatrix.data);
			glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
			Vector4f colorFilter = context->renderContext->colorFilter;
			glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		} else {
			Vector4f colorFilter = context->renderContext->colorFilter;
			glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
			glVertexPointer(3, GL_FLOAT, 0, &verts);
			glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		}
		if (renderChars > 0) {
			glDrawArrays(GL_TRIANGLES, 0, renderChars * 6);
		}
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

}
