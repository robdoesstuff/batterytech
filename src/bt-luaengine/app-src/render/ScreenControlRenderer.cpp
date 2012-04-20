/*
 * ScreenControlRenderer.cpp
 *
 *  Created on: May 28, 2011
 *      Author: rgreen
 */

#include "ScreenControlRenderer.h"
#include <batterytech/render/GLResourceManager.h>
#include <batterytech/render/RenderContext.h>
#include "../World.h"
#include "../ScreenControl.h"
#include <batterytech/render/TextRasterRenderer.h>
#include <batterytech/util/strx.h>
#include "WorldRenderer.h"
#include <batterytech/Logger.h>
#include <stdio.h>
#include "../GameContext.h"

ScreenControlRenderer::ScreenControlRenderer(GameContext *context, const char *fontTag) {
	this->context = context;
	shaderProgram = new ShaderProgram("shaders/quadshader.vert", "shaders/quadshader.frag");
	this->fontTag = strDuplicate(fontTag);
}

ScreenControlRenderer::~ScreenControlRenderer() {
	delete shaderProgram;
	delete [] fontTag;
}

void ScreenControlRenderer::init(BOOL32 newContext) {
	if (!newContext) {
		return;
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
	checkGLError("ScreenControlRenderer Init");
}

void ScreenControlRenderer::render() {
	//glFrontFace(GL_CW);
	shaderProgram->bind();
	ManagedArray<ScreenControl> *controls = context->world->screenControls;
	for (S32 i = 0; i < controls->getSize(); i++) {
		ScreenControl *control = controls->array[i];
		if (!control->getTextureAssetName()) {
			continue;
		}
		context->glResourceManager->getTexture(control->getTextureAssetName())->bind();
		Vector4f &bounds = control->drawableBounds;
		F32 left = bounds.x;
		F32 top = bounds.y;
		F32 right = bounds.z;
		F32 bottom = bounds.w;
		F32 verts[] = {
				left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
		};
		Vector4f &uvCoords = control->textureUVs;
		F32 uvLeft = uvCoords.x;
		F32 uvTop = uvCoords.y;
		F32 uvRight = uvCoords.z;
		F32 uvBottom = uvCoords.w;
		F32 uvs[] = {
				uvLeft, uvTop, uvRight, uvTop, uvRight, uvBottom, uvLeft, uvBottom
		};
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, 0, uvs);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->mvMatrix.data);
		glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
		Vector4f colorFilter = context->renderContext->colorFilter;
		glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
	shaderProgram->unbind();
	
	BOOL32 hasLabels = FALSE;

	for (S32 i = 0; i < controls->getSize(); i++) {
		ScreenControl *control = controls->array[i];
		if (!control->getTextureAssetName()) {
			continue;
		}
		if (control->getLabel() && strlen(control->getLabel()) > 0) {
			hasLabels = TRUE;
		}
	}
	if (hasLabels) {
		TextRasterRenderer *textRenderer = context->worldRenderer->getTextRenderer(fontTag);
		if (textRenderer) {
			textRenderer->startText();
			F32 th = textRenderer->getHeight();
			for (S32 i = 0; i < controls->getSize(); i++) {
				ScreenControl *control = controls->array[i];
				if (!control->getTextureAssetName() || !control->getLabel() || strlen(control->getLabel()) == 0) {
					continue;
				}
				F32 tw = textRenderer->measureWidth(control->getLabel());
				Vector4f &bounds = control->drawableBounds;
				F32 left = bounds.x;
				F32 top = bounds.y;
				F32 right = bounds.z;
				F32 bottom = bounds.w;
				F32 diff = tw / (right - left);
				F32 scale = 1.0f;
				if (diff > .9f) {
					// reduce label to stay inside button
					scale = .9f / diff;
					tw = (right - left) * .9f;
				}

				F32 textX, textY;
				textX = left + ((right - left)/2);
				textY = top + ((bottom - top)/2.6);
				textX -= tw/2;
				textY += th/2;
				textRenderer->render(control->getLabel(), textX, textY, scale);
			}
			textRenderer->finishText();
		} else {
			char buf[255];
			sprintf(buf, "Error - ScreenControlRenderer can't find font tagged [%s]", fontTag);
			logmsg(buf);
		}
	}
}
