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
#include "../Context.h"
#include "../render/RenderContext.h"

TextRasterRenderer::TextRasterRenderer(Context *context, const char *assetName, float fontSize) {
	this->context = context;
	aName = assetName;
	vertShader = fragShader = program = shaderProjMatrix =
	shaderMVMatrix = shaderVPosition = shaderUvMap = shaderTex =
	shaderColorFilter = 0;
}

void TextRasterRenderer::init(BOOL32 newContext) {
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
	// longest string is 255
	F32 verts[255 * 6 * 3];
	F32 uvs[255 * 6 * 2];
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

TextRasterRenderer::~TextRasterRenderer() {
	// TODO Auto-generated destructor stub
}
