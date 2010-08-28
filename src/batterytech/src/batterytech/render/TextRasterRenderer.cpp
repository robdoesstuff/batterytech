/*
 * TextRasterRenderer.cpp
 *
 *  Created on: Aug 27, 2010
 *      Author: rgreen
 */

#include "TextRasterRenderer.h"
#include "../platform/platformgeneral.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../decoders/stb_truetype.h"
#include "../logger.h"
#include "../batterytech.h"

unsigned char temp_bitmap[512*512];
unsigned char temp_bitmap_rgba[512*512*4];

stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
stbtt_uint32 ftex;
const char *aName;
unsigned char *data;

TextRasterRenderer::TextRasterRenderer(const char *assetName, float fontSize) {
	aName = assetName;
}

void TextRasterRenderer::init() {
	int size = 0;
	data = _platform_load_asset(aName, &size);
	if (data) {
		stbtt_BakeFontBitmap(data, 0, 32.0, temp_bitmap, 512, 512, 32,96, cdata); // no guarantee this fits!
		int i,j;
		for (i = 0; i < 512; i++) {
			for (j = 0; j < 512; j++) {
				int rowStart = i * 512 * 4;
				char alpha = temp_bitmap[i * 512 + j];
				// will give a nice b&w gradient
				temp_bitmap_rgba[rowStart + j * 4] = alpha;
				temp_bitmap_rgba[rowStart + j * 4 + 1] = alpha;
				temp_bitmap_rgba[rowStart + j * 4 + 2] = alpha;
				temp_bitmap_rgba[rowStart + j * 4 + 3] = alpha;
			}
		}
		// can free ttf_buffer at this point
		glGenTextures(1, &ftex);
		glBindTexture(GL_TEXTURE_2D, ftex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_bitmap_rgba);
		GLenum error = glGetError();
		if (error) {
			logger::logMsg("GL Error");
		}
		// can free temp_bitmap at this point
		_platform_free_asset(data);
	} else {
		logger::logMsg("error loading font");
	}

}


void TextRasterRenderer::loadLevel() {
}

void TextRasterRenderer::unloadLevel() {
}

void TextRasterRenderer::render() {
	char *text = "THIS IS SOME TEXT";
	//logger::logMsg("rendering text()");
	// assume orthographic projection with units = screen pixels, origin at top left
	float x = 300;
	float y = 200;
	int length = strlen(text);
	float verts[length * 6 * 3];
	float uvs[length * 6 * 2];
	float colors[length * 6 * 4];
	int i = 0;
	for (i = 0; i < length * 6 * 4; i++) {
		colors[i] = 1.0f;
	}
	i = 0;
	while (*text) {
		if (*text >= 32 && *text < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl,0=old d3d
			int pos = i * 18;
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
	glFrontFace(GL_CW);
	glBindTexture(GL_TEXTURE_2D, ftex);
	glColorPointer(4, GL_FLOAT, 0, &colors);
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
