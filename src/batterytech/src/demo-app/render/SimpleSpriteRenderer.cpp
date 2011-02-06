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
	//char buf[50];
	//sprintf(buf, "Loaded texture Id=%d", textureId);
	//logmsg(buf);
}

void SimpleSpriteRenderer::setSpriteAssetname(const char* spriteAssetName) {
	this->spriteAssetName = spriteAssetName;
}

void SimpleSpriteRenderer::render(F32 top, F32 right, F32 bottom, F32 left) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	drawTexturedQuad(top, right, bottom, left);
}

void SimpleSpriteRenderer::render(F32 x, F32 y, F32 width, F32 height, F32 angleRads) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
	drawTexturedQuad(height/2, width/2, -height/2, -width/2);
	glPopMatrix();
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
