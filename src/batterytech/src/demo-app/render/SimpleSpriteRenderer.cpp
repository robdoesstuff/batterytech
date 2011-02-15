/*
 * SimpleSpriteRenderer.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: rgreen
 */

#include "SimpleSpriteRenderer.h"
#include "../GameConstants.h"
#include <stdio.h>
#include <string.h>
#include <batterytech/Logger.h>
#include <batterytech/util/esTransform.h>
#include <batterytech/render/GraphicsConfiguration.h>
#include <batterytech/render/RenderContext.h>

GLuint vShader, fShader, program, projMatrixUniform, mvMatrixUniform, vertPosition, texPosition, uvmapPosition;

SimpleSpriteRenderer::SimpleSpriteRenderer(Context *context, const char *spriteAssetName) {
	this->context = context;
	this->spriteAssetName = spriteAssetName;
	textureId = 0;
}

SimpleSpriteRenderer::~SimpleSpriteRenderer() {
}

void SimpleSpriteRenderer::init(BOOL32 newContext) {
	if (!newContext && textureId) {
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
	if (spriteAssetName && strlen(spriteAssetName) > 0) {
		textureId = loadTexture(spriteAssetName);
	}
	if (context->gConfig->useShaders) {
		GLint status = 0;
		vShader = loadShaderFromAsset(GL_VERTEX_SHADER, "shaders/quadshader.vert");
		fShader = loadShaderFromAsset(GL_FRAGMENT_SHADER, "shaders/quadshader.frag");
		program = glCreateProgram();
		glAttachShader(program, vShader);
		glAttachShader(program, fShader);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if(status == GL_FALSE){
			logProgramInfo(program);
		}
		vertPosition = glGetAttribLocation(program, "vPosition");
		uvmapPosition = glGetAttribLocation(program, "uvMap");
		projMatrixUniform = glGetUniformLocation(program, "projection_matrix");
		mvMatrixUniform = glGetUniformLocation(program, "modelview_matrix");
		texPosition = glGetUniformLocation(program, "tex");
	}
}

void SimpleSpriteRenderer::setSpriteAssetname(const char* spriteAssetName) {
	this->spriteAssetName = spriteAssetName;
}

void SimpleSpriteRenderer::render(F32 top, F32 right, F32 bottom, F32 left) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	drawTexturedQuad(top, right, bottom, left);
}

void SimpleSpriteRenderer::render(F32 x, F32 y, F32 width, F32 height, F32 angleRads) {
	// this one
	glBindTexture(GL_TEXTURE_2D, textureId);
	if (context->gConfig->useShaders) {
		glUseProgram(program);
		F32 top = height/2;
		F32 right = width/2;
		F32 bottom = -height/2;
		F32 left = -width/2;
		F32 verts[] = {
				left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
		};
		F32 uvs[] = {
				0, 0, 1, 0, 1, 1, 0, 1
		};
		ESMatrix myMvMatrix;
		esCopy(&myMvMatrix, &context->renderContext->mvMatrix);
		esTranslate(&myMvMatrix, x, y, 0);
		esRotate(&myMvMatrix, angleRads * (180 / PI), 0, 0, -1.0f);
		glEnableVertexAttribArray(vertPosition);
		glEnableVertexAttribArray(uvmapPosition);
		glVertexAttribPointer(vertPosition, 3, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(uvmapPosition, 2, GL_FLOAT, GL_FALSE, 0, uvs);
		glUniformMatrix4fv(projMatrixUniform, 1, GL_FALSE, (GLfloat*) &context->renderContext->projMatrix.m[0][0]);
		glUniformMatrix4fv(mvMatrixUniform, 1, GL_FALSE, (GLfloat*) &myMvMatrix.m[0][0]);
		glUniform1i(texPosition, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glFrontFace(GL_CW);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDisableVertexAttribArray(vertPosition);
		glDisableVertexAttribArray(uvmapPosition);
		glUseProgram(0);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	// GL1 rendering branch
	//glFrontFace(GL_CW);
	//glPushMatrix();
	//glTranslatef(x, y, 0);
	//glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
	//glVertexPointer(3, GL_FLOAT, 0, &verts);
	//glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//glPopMatrix();

}

void SimpleSpriteRenderer::renderUVLimited(F32 x, F32 y, F32 width, F32 height, F32 angleRads, F32 uvTop, F32 uvBottom) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
	F32 top = height/2;
	F32 right = width/2;
	F32 bottom = -height/2;
	F32 left = -width/2;
	F32 verts[] = {
			left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
	};
	F32 uvs[] = {
			0, uvTop, 1, uvTop, 1, uvBottom, 0, uvBottom
	};
	glFrontFace(GL_CW);
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glPopMatrix();
}

void SimpleSpriteRenderer::renderUVLimitedSpecific(F32 top, F32 right, F32 bottom, F32 left, F32 x, F32 y, F32 angleRads, F32 uvTop, F32 uvBottom) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
	F32 verts[] = {
			left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
	};
	F32 uvs[] = {
			0, uvTop, 1, uvTop, 1, uvBottom, 0, uvBottom
	};
	glFrontFace(GL_CW);
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glPopMatrix();
}

void SimpleSpriteRenderer::drawTexturedQuad(F32 top, F32 right, F32 bottom, F32 left) {
	F32 verts[] = {
			left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
	};
	F32 uvs[] = {
			0, 0, 1, 0, 1, 1, 0, 1
	};
	glFrontFace(GL_CW);
	glVertexPointer(3, GL_FLOAT, 0, &verts);
	glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}



