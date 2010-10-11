/*
 * TextRasterRenderer.cpp
 *
 *  Created on: Aug 27, 2010
 *      Author: rgreen
 */

#include "TextRasterRenderer.h"
#include "../platform/platformgeneral.h"
#include "../platform/platformgl.h"
#include "../primitives.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../decoders/stb_truetype.h"
#include "../Logger.h"

TextRasterRenderer::TextRasterRenderer(GraphicsConfiguration *gConfig, const char *assetName, float fontSize) {
	aName = assetName;
}

void TextRasterRenderer::init() {
	S32 size = 0;
	BYTE8 *data;
	data = _platform_load_asset(aName, &size);
	BYTE8 *temp_bitmap = (unsigned char*) malloc(sizeof(unsigned char) * 512*512);
	BYTE8 *temp_bitmap_rgba = (unsigned char*) malloc(sizeof(unsigned char) * 512*512*4);
	if (data) {
		stbtt_BakeFontBitmap(data, 0, 32.0, temp_bitmap, 512, 512, 32,96, cdata); // no guarantee this fits!
		S32 i,j;
		for (i = 0; i < 512; i++) {
			for (j = 0; j < 512; j++) {
				S32 rowStart = i * 512 * 4;
				BYTE8 alpha = temp_bitmap[i * 512 + j];
				// will give a nice b&w gradient
				temp_bitmap_rgba[rowStart + j * 4] = alpha;
				temp_bitmap_rgba[rowStart + j * 4 + 1] = alpha;
				temp_bitmap_rgba[rowStart + j * 4 + 2] = alpha;
				temp_bitmap_rgba[rowStart + j * 4 + 3] = alpha;
			}
		}
		// can free ttf_buffer at this point
		free(temp_bitmap);
		glGenTextures(1, &ftex);
		glBindTexture(GL_TEXTURE_2D, ftex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_bitmap_rgba);
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

}


void TextRasterRenderer::loadLevel() {
}

void TextRasterRenderer::unloadLevel() {
}

F32 TextRasterRenderer::getHeight() {
	F32 x = 0;
	F32 y = 0;
	stbtt_aligned_quad q;
	stbtt_GetBakedQuad(cdata, 512,512, 'A'-32, &x,&y,&q,1);//1=opengl,0=old d3d
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
			stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl,0=old d3d
			if (q.x0 < lowestX) {
				lowestX = q.x0;
			}
			if (q.x1 > highestX) {
				highestX = q.x1;
			}
			++text;
		}
	}
	return highestX - lowestX;
}

void TextRasterRenderer::render(const char *text, F32 x, F32 y) {
	glColor4f(1, 1, 1, 1);
	glFrontFace(GL_CW);
	glBindTexture(GL_TEXTURE_2D, ftex);
	S32 length = strlen(text);
	F32 verts[length * 6 * 3];
	F32 uvs[length * 6 * 2];
	S32 i = 0;
	while (*text) {
		if (*text >= 32 && *text < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl,0=old d3d
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
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, length * 6);
}

void TextRasterRenderer::render(World *world) {
	const char *text = "BATTERYTECH v1.0";
	//logger::logMsg("rendering text()");
	// assume orthographic projection with units = screen pixels, origin at top left
	F32 x = 350;
	F32 y = 50;
	S32 length = strlen(text);
	F32 verts[length * 6 * 3];
	F32 uvs[length * 6 * 2];
	S32 i = 0;
	while (*text) {
		if (*text >= 32 && *text < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl,0=old d3d
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, 800, 480, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glColor4f(1, 1, 1, 1);
	glFrontFace(GL_CW);
	glBindTexture(GL_TEXTURE_2D, ftex);
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, length * 6);
	glDisable(GL_BLEND);
}

TextRasterRenderer::~TextRasterRenderer() {
	// TODO Auto-generated destructor stub
}
