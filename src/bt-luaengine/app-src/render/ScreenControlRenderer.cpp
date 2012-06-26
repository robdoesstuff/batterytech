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
#include <batterytech/render/QuadRenderer.h>

ScreenControlRenderer::ScreenControlRenderer(GameContext *context, const char *fontTag) {
	this->context = context;
	this->fontTag = strDuplicate(fontTag);
}

ScreenControlRenderer::~ScreenControlRenderer() {
	delete [] fontTag;
}

void ScreenControlRenderer::init(BOOL32 newContext) {
	if (!newContext) {
		return;
	}
	checkGLError("ScreenControlRenderer Init");
}

void ScreenControlRenderer::render() {
	//glFrontFace(GL_CW);
	ManagedArray<ScreenControl> *controls = context->world->screenControls;
	for (S32 i = 0; i < controls->getSize(); i++) {
		ScreenControl *control = controls->array[i];
		if (!control->getTextureAssetName()) {
			continue;
		}
		Texture *texture = context->glResourceManager->getTexture(control->getTextureAssetName());
		Vector4f &bounds = control->drawableBounds;
		F32 left = bounds.x;
		F32 top = bounds.y;
		F32 right = bounds.z;
		F32 bottom = bounds.w;
		F32 width = right-left;
		F32 height = bottom-top;
		context->quadRenderer->render(texture, Vector3f(left + width/2, top + height/2, 0), 0, control->textureUVs, Vector2f(width, height), Vector4f(1,1,1,1), TRUE, FALSE, Matrix4f());
	}
	
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
