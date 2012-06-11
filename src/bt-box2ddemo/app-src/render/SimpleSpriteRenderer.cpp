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
#include "../GameContext.h"
#include <batterytech/render/RenderContext.h>

SimpleSpriteRenderer::SimpleSpriteRenderer(GameContext *context, const char *spriteAssetName) {
	this->context = context;
	this->spriteAssetName = spriteAssetName;
	textureId = 0;
	shaderProgram = new ShaderProgram("quad", "shaders/quadshader.vert", "shaders/quadshader.frag");
}

SimpleSpriteRenderer::~SimpleSpriteRenderer() {
	delete shaderProgram;
}

void SimpleSpriteRenderer::init(BOOL32 newGameContext) {
	if (!newGameContext && textureId) {
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
	if (spriteAssetName && strlen(spriteAssetName) > 0) {
		textureId = loadTexture(spriteAssetName, context);
	}
	if (context->gConfig->useShaders) {
		shaderProgram->init(newGameContext);
		shaderProgram->addVertexAttributeLoc("vPosition");
		shaderProgram->addVertexAttributeLoc("uvMap");
		shaderProgram->addUniformLoc("projection_matrix");
		shaderProgram->addUniformLoc("modelview_matrix");
		shaderProgram->addUniformLoc("tex");
		shaderProgram->addUniformLoc("colorFilter");
	}
}

void SimpleSpriteRenderer::setSpriteAssetname(const char* spriteAssetName) {
	this->spriteAssetName = spriteAssetName;
}

void SimpleSpriteRenderer::render(F32 top, F32 right, F32 bottom, F32 left) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	F32 verts[] = {
			left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
	};
	F32 uvs[] = {
			0, 0, 1, 0, 1, 1, 0, 1
	};
	glFrontFace(GL_CW);
	if (context->gConfig->useShaders) {
		shaderProgram->bind();
		glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
		Vector4f colorFilter = context->renderContext->colorFilter;
		glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.r,colorFilter.g,colorFilter.b,colorFilter.a);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, 0, uvs);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->mvMatrix.data);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		shaderProgram->unbind();
	} else {
		glVertexPointer(3, GL_FLOAT, 0, &verts);
		glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
}

void SimpleSpriteRenderer::render(F32 x, F32 y, F32 width, F32 height, F32 angleRads) {
	// this one
	glBindTexture(GL_TEXTURE_2D, textureId);
	glFrontFace(GL_CW);
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
		shaderProgram->bind();
		Matrix4f myMvMatrix = context->renderContext->mvMatrix;
		myMvMatrix.translate(x, y, 0);
		myMvMatrix.rotate(angleRads * (180 / PI), 0, 0, -1.0f);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, 0, uvs);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) myMvMatrix.data);
		glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
		Vector4f colorFilter = context->renderContext->colorFilter;
		glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.r,colorFilter.g,colorFilter.b,colorFilter.a);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		shaderProgram->unbind();
	} else {
		// GL1 rendering branch
		Vector4f colorFilter = context->renderContext->colorFilter;
		glColor4f(colorFilter.r,colorFilter.g,colorFilter.b,colorFilter.a);
		glPushMatrix();
		glTranslatef(x, y, 0);
		glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
		glVertexPointer(3, GL_FLOAT, 0, &verts);
		glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
	}

}



