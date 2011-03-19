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
#include "../primitives.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "../decoders/stb_truetype.h"
#include "../Logger.h"
#include "../Context.h"
#include "../render/RenderContext.h"
#include "../batterytech_globals.h"
#include <stdio.h>

namespace BatteryTech {

	TextRasterRenderer::TextRasterRenderer(Context *context, const char *assetName, F32 fontSize) {
		this->context = context;
		aName = assetName;
		this->fontSize = fontSize * context->gConfig->uiScale;
		bmpWidth = 0;
		bmpHeight = 0;
		vertShader = fragShader = program = shaderProjMatrix =
		shaderMVMatrix = shaderVPosition = shaderUvMap = shaderTex =
		shaderColorFilter = 0;
	}

	TextRasterRenderer::~TextRasterRenderer() {
	}

	void TextRasterRenderer::init(BOOL32 newContext) {
		S32 bmpWidth = INITIAL_FONT_TEXTURE_WIDTH;
		S32 bmpHeight = INITIAL_FONT_TEXTURE_HEIGHT;
		S32 size = 0;
		BYTE8 *data;
		data = _platform_load_asset(aName, &size);
		if (data) {
			BYTE8 *temp_bitmap = NULL;
			BYTE8 *temp_bitmap_rgba = NULL;
			BOOL32 fit = FALSE;
			BOOL32 increaseToggle = FALSE;
			while (!fit) {
				// this isn't the most efficient way to do this but anything better will require modifying stb_truetype to do a dry run with no allocation
				temp_bitmap = (unsigned char*) malloc(sizeof(unsigned char) * bmpWidth*bmpHeight);
				temp_bitmap_rgba = (unsigned char*) malloc(sizeof(unsigned char) * bmpWidth*bmpHeight*4);
				S32 result = stbtt_BakeFontBitmap(data, 0, fontSize, temp_bitmap, bmpWidth, bmpHeight, 32,95, cdata); // no guarantee this fits!
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
			/*
			// uncomment for debugging font texture generation
			FILE *testOutFile;
			testOutFile = fopen("bmp8.raw", "wb");
			fwrite(temp_bitmap, bmpWidth * bmpHeight, 1, testOutFile);
			fclose(testOutFile);
			testOutFile = fopen("bmp32.raw", "wb");
			fwrite(temp_bitmap_rgba, bmpWidth * bmpHeight * 4, 1, testOutFile);
			fclose(testOutFile);
			*/
			// can free ttf_buffer at this point
			free(temp_bitmap);
			glGenTextures(1, &ftex);
			glBindTexture(GL_TEXTURE_2D, ftex);
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
		if (!newContext && program) {
			glDetachShader(program, vertShader);
			glDetachShader(program, fragShader);
			glDeleteShader(vertShader);
			glDeleteShader(fragShader);
			glDeleteProgram(program);
		}
		if (context->gConfig->useShaders) {
			GLint status = 0;
			vertShader = loadShaderFromAsset(GL_VERTEX_SHADER, "shaders/quadshader.vert");
			fragShader = loadShaderFromAsset(GL_FRAGMENT_SHADER, "shaders/quadshader.frag");
			program = glCreateProgram();
			glAttachShader(program, vertShader);
			glAttachShader(program, fragShader);
			glLinkProgram(program);
			glGetProgramiv(program, GL_LINK_STATUS, &status);
			if(status == GL_FALSE){
				logProgramInfo(program);
			}
			shaderVPosition = glGetAttribLocation(program, "vPosition");
			shaderUvMap = glGetAttribLocation(program, "uvMap");
			shaderProjMatrix = glGetUniformLocation(program, "projection_matrix");
			shaderMVMatrix = glGetUniformLocation(program, "modelview_matrix");
			shaderTex = glGetUniformLocation(program, "tex");
			shaderColorFilter = glGetUniformLocation(program, "colorFilter");
		}

	}

	F32 TextRasterRenderer::getHeight() {
		F32 x = 0;
		F32 y = 0;
		stbtt_aligned_quad q;
		stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, 'A'-32, &x,&y,&q,1);//1=opengl,0=old d3d
		return q.y1 - q.y0;
	}

	F32 TextRasterRenderer::measureWidth(const char *text) {
		F32 x = 0;
		F32 y = 0;
		// high/low diff is the way to go since text often overlaps with non-fixed-width fonts
		F32 lowestX = 999999;
		F32 highestX = 0;
		while (*text) {
			if (*text >= 32 && *text < 128) {
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, *text-32, &x,&y,&q,1);//1=opengl,0=old d3d
				if (q.x0 < lowestX) {
					lowestX = q.x0;
				}
				if (q.x1 > highestX) {
					highestX = q.x1;
				}
			}
			++text;
		}
		return highestX - lowestX;
	}

	void TextRasterRenderer::startText() {
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glFrontFace(GL_CW);
		glBindTexture(GL_TEXTURE_2D, ftex);
		if (context->gConfig->useShaders) {
			glUseProgram(program);
			glEnableVertexAttribArray(shaderVPosition);
			glEnableVertexAttribArray(shaderUvMap);
		}
	}

	void TextRasterRenderer::finishText() {
		//glDisable(GL_BLEND);
		if (context->gConfig->useShaders) {
			glDisableVertexAttribArray(shaderVPosition);
			glDisableVertexAttribArray(shaderUvMap);
			glUseProgram(0);
		}
	}


	void TextRasterRenderer::render(const char *text, F32 x, F32 y) {
		S32 length = strlen(text);
		if (length > TEXT_RENDER_MAX_LINE_LENGTH) {
			length = TEXT_RENDER_MAX_LINE_LENGTH;
		}
		// longest string is TEXT_RENDER_MAX_LINE_LENGTH
		F32 verts[TEXT_RENDER_MAX_LINE_LENGTH * 6 * 3];
		F32 uvs[TEXT_RENDER_MAX_LINE_LENGTH * 6 * 2];
		S32 i = 0;
		while (*text) {
			if (*text >= 32 && *text < 128) {
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, *text-32, &x,&y,&q,1);//1=opengl,0=old d3d
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
			glVertexAttribPointer(shaderVPosition, 3, GL_FLOAT, GL_FALSE, 0, verts);
			glVertexAttribPointer(shaderUvMap, 2, GL_FLOAT, GL_FALSE, 0, uvs);
			glUniformMatrix4fv(shaderProjMatrix, 1, GL_FALSE, (GLfloat*) &context->renderContext->projMatrix.m[0][0]);
			glUniformMatrix4fv(shaderMVMatrix, 1, GL_FALSE, (GLfloat*) &context->renderContext->mvMatrix.m[0][0]);
			glUniform1i(shaderTex, 0);
			Vec4f colorFilter = context->renderContext->colorFilter;
			glUniform4f(shaderColorFilter, colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		} else {
			Vec4f colorFilter = context->renderContext->colorFilter;
			glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
			glVertexPointer(3, GL_FLOAT, 0, &verts);
			glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		}
		glDrawArrays(GL_TRIANGLES, 0, length * 6);
	}

	F32 TextRasterRenderer::measureMultilineHeight(const char *text, F32 availableWidth) {
		S32 i = 0;
		S32 lastSpaceIdx = -1;
		F32 lineHeight = getHeight() * TEXT_VERTICAL_SPACING_MULT;
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
			if (c >= 32 && c < 128) {
				if (c == 32) {
					lastSpaceIdx = i;
				}
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, c-32, &x,&y,&q,1);//1=opengl,0=old d3d
				if (x > availableWidth) {
					x = 0;
					y += lineHeight;
					// rewind to 1 past last space
					if (lastSpaceIdx != -1) {
						i = lastSpaceIdx + 1;
						lastSpaceIdx = -1;
						c = *(text + i);
					}
					stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, c-32, &x,&y,&q,1);//1=opengl,0=old d3d
				}
			}
			++i;
			c = *(text + i);
		}
		return y;
	}

	void TextRasterRenderer::renderMultiline(const char *text, F32 x, F32 y, F32 maxX, F32 maxY) {
		S32 length = strlen(text);
		if (length > TEXT_RENDER_MAX_MULTILINE_LENGTH) {
			length = TEXT_RENDER_MAX_MULTILINE_LENGTH;
		}
		// longest string is TEXT_RENDER_MAX_MULTILINE_LENGTH
		F32 verts[TEXT_RENDER_MAX_MULTILINE_LENGTH * 6 * 3];
		F32 uvs[TEXT_RENDER_MAX_MULTILINE_LENGTH * 6 * 2];
		S32 i = 0;
		S32 lastSpaceIdx = -1;
		F32 origX = x;
		//F32 origY = y;
		F32 lineHeight = getHeight() * TEXT_VERTICAL_SPACING_MULT;
		char c = *text;
		while (c) {
			if (c == '\n') {
				lastSpaceIdx = -1;
				y += lineHeight;
				x = origX;
				if (y > maxY) {
					break;
				}
			}
			if (c >= 32 && c < 128) {
				if (c == 32) {
					lastSpaceIdx = i;
				}
				stbtt_aligned_quad q;
				stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, c-32, &x,&y,&q,1);//1=opengl,0=old d3d
				if (x > maxX) {
					x = origX;
					y += lineHeight;
					// rewind to 1 past last space
					if (lastSpaceIdx != -1) {
						i = lastSpaceIdx + 1;
						lastSpaceIdx = -1;
						c = *(text + i);
					}
					if (y > maxY) {
						break;
					} else {
						stbtt_GetBakedQuad(cdata, bmpWidth, bmpHeight, c-32, &x,&y,&q,1);//1=opengl,0=old d3d
					}
				}
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
			++i;
			c = *(text + i);
		}
		if (context->gConfig->useShaders) {
			glVertexAttribPointer(shaderVPosition, 3, GL_FLOAT, GL_FALSE, 0, verts);
			glVertexAttribPointer(shaderUvMap, 2, GL_FLOAT, GL_FALSE, 0, uvs);
			glUniformMatrix4fv(shaderProjMatrix, 1, GL_FALSE, (GLfloat*) &context->renderContext->projMatrix.m[0][0]);
			glUniformMatrix4fv(shaderMVMatrix, 1, GL_FALSE, (GLfloat*) &context->renderContext->mvMatrix.m[0][0]);
			glUniform1i(shaderTex, 0);
			Vec4f colorFilter = context->renderContext->colorFilter;
			glUniform4f(shaderColorFilter, colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		} else {
			Vec4f colorFilter = context->renderContext->colorFilter;
			glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
			glVertexPointer(3, GL_FLOAT, 0, &verts);
			glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		}
		glDrawArrays(GL_TRIANGLES, 0, length * 6);
	}

}
