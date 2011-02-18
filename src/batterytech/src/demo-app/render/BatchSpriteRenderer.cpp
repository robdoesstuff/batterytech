/*
 * BatchSpriteRenderer
 *
 *  Created on: Dec 2, 2010
 *      Author: rgreen
 */

#include "BatchSpriteRenderer.h"
#include "../GameConstants.h"
#include <stdio.h>
#include <string.h>
#include <batterytech/Logger.h>
#include <batterytech/util/esTransform.h>
#include <batterytech/render/GraphicsConfiguration.h>
#include <batterytech/render/RenderContext.h>

BatchSpriteRenderer::BatchSpriteRenderer(Context *context, const char *spriteAssetName) {
	this->context = context;
	this->spriteAssetName = spriteAssetName;
	textureId = 0;
	vertShader = fragShader = program = shaderProjMatrix =
	shaderMVMatrix = shaderVPosition = shaderUvMap = shaderTex =
	shaderColorFilter = 0;
}

BatchSpriteRenderer::~BatchSpriteRenderer() {
}

void BatchSpriteRenderer::init(BOOL32 newContext) {
	if (!newContext && textureId) {
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
	if (spriteAssetName && strlen(spriteAssetName) > 0) {
		textureId = loadTexture(spriteAssetName, context->gConfig->textureFilter);
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

void BatchSpriteRenderer::setSpriteAssetname(const char* spriteAssetName) {
	this->spriteAssetName = spriteAssetName;
}

void BatchSpriteRenderer::startBatch() {
	glBindTexture(GL_TEXTURE_2D, textureId);
	glFrontFace(GL_CW);
	if (context->gConfig->useShaders) {
		glUseProgram(program);
		glEnableVertexAttribArray(shaderVPosition);
		glEnableVertexAttribArray(shaderUvMap);
		glUniform1i(shaderTex, 0);
		Vec4f colorFilter = context->renderContext->colorFilter;
		glUniform4f(shaderColorFilter, colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
	}
}

void BatchSpriteRenderer::endBatch() {
	if (context->gConfig->useShaders) {
		glDisableVertexAttribArray(shaderVPosition);
		glDisableVertexAttribArray(shaderUvMap);
		glUseProgram(0);
	}
}

void BatchSpriteRenderer::render(F32 top, F32 right, F32 bottom, F32 left) {
	F32 verts[] = {
			left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
	};
	F32 uvs[] = {
			0, 0, 1, 0, 1, 1, 0, 1
	};
	if (context->gConfig->useShaders) {
		glVertexAttribPointer(shaderVPosition, 3, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(shaderUvMap, 2, GL_FLOAT, GL_FALSE, 0, uvs);
		glUniformMatrix4fv(shaderProjMatrix, 1, GL_FALSE, (GLfloat*) &context->renderContext->projMatrix.m[0][0]);
		glUniformMatrix4fv(shaderMVMatrix, 1, GL_FALSE, (GLfloat*) &context->renderContext->mvMatrix.m[0][0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, &verts);
		glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
}

void BatchSpriteRenderer::render(F32 x, F32 y, F32 width, F32 height, F32 angleRads) {
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
	if (context->gConfig->useShaders) {
		ESMatrix myMvMatrix;
		esCopy(&myMvMatrix, &context->renderContext->mvMatrix);
		esTranslate(&myMvMatrix, x, y, 0);
		esRotate(&myMvMatrix, angleRads * (180 / PI), 0, 0, -1.0f);
		glVertexAttribPointer(shaderVPosition, 3, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(shaderUvMap, 2, GL_FLOAT, GL_FALSE, 0, uvs);
		glUniformMatrix4fv(shaderProjMatrix, 1, GL_FALSE, (GLfloat*) &context->renderContext->projMatrix.m[0][0]);
		glUniformMatrix4fv(shaderMVMatrix, 1, GL_FALSE, (GLfloat*) &myMvMatrix.m[0][0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	} else {
		// GL1 rendering branch
		Vec4f colorFilter = context->renderContext->colorFilter;
		glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		glPushMatrix();
		glTranslatef(x, y, 0);
		glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
		glVertexPointer(3, GL_FLOAT, 0, &verts);
		glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
	}
}


