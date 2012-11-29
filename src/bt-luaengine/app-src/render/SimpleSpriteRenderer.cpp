/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

#include "SimpleSpriteRenderer.h"
#include "../GameConstants.h"
#include <stdio.h>
#include <string.h>
#include <batterytech/Logger.h>
#include <batterytech/util/esTransform.h>
#include <batterytech/render/GraphicsConfiguration.h>
#include <batterytech/render/RenderContext.h>
#include <batterytech/render/GLResourceManager.h>
#include "../World.h"
#include "../GameContext.h"
#include <batterytech/render/QuadRenderer.h>

SimpleSpriteRenderer::SimpleSpriteRenderer(GameContext *context) {
	this->context = context;
}

SimpleSpriteRenderer::~SimpleSpriteRenderer() {
}

void SimpleSpriteRenderer::init(BOOL32 newContext) {
}

void SimpleSpriteRenderer::render(RenderItem *item) {
	Texture *texture = NULL;
	if (item->textureName[0]) {
		texture = context->glResourceManager->getTexture(item->textureName);
		if (!texture) {
			char buf[1024];
			sprintf(buf, "Texture not found %s", item->textureName);
			logmsg(buf);
		}
	}
	Matrix4f bbMat;
	BOOL32 isBB = item->renderType == RenderItem::RENDERTYPE_BB;
	Vector4f newUvs = item->uvs;
	if (isBB) {
		// calculate billboard matrix
		Vector3f dir = context->world->camera->pos - item->pos;
		dir.normalize();
		Vector3f newY = context->world->camera->invRotMatrix * Vector3f(0,1,0);
		Vector3f newX = context->world->camera->invRotMatrix * Vector3f(1,0,0);
		bbMat.data[0] = newX.x;
		bbMat.data[1] = newX.y;
		bbMat.data[2] = newX.z;
		bbMat.data[4] = newY.x;
		bbMat.data[5] = newY.y;
		bbMat.data[6] = newY.z;
		bbMat.data[8] = dir.x;
		bbMat.data[9] = dir.y;
		bbMat.data[10] = dir.z;
		// uvs will need flipping to avoid switching to LHS to flip Y
		newUvs.y = newUvs.w;
		newUvs.w = item->uvs.y;
	}
	context->quadRenderer->render(texture, item->pos, item->orientation.v.z, newUvs, Vector2f(item->scale.x, item->scale.y), item->colorFilter, item->flags & RENDERITEM_FLAG_IS_OPAQUE, isBB, bbMat);
}

void SimpleSpriteRenderer::startBatch() {
    context->quadRenderer->startBatch();
}

void SimpleSpriteRenderer::endBatch() {
    context->quadRenderer->endBatch();
}
