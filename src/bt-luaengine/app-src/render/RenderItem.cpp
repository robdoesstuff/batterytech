/*
 * RenderItem.cpp
 *
 *  Created on: Jul 11, 2011
 *      Author: rgreen
 */

#include "RenderItem.h"
#include <batterytech/render/AssimpAnimator.h>

RenderItem::RenderItem() {
	reset();
}

RenderItem::~RenderItem() {
}

void RenderItem::reset() {
	textureName[0] = '\0';
	resourceName[0] = '\0';
	attr1[0] = '\0';
	attr2[0] = '\0';
	mat.identity();
	pos = Vector3f(0,0,0);
	scale = Vector3f(0,0,0);
	orientation = Quatf(0,0,0,0);
	uvs = Vector4f(0,0,1,1);
	camDist = 0;
	animator = NULL;
	maxPointLights = 0;
	flags = 0;
	alpha = 1.0f;
	viewport = Vector4i(0,0,0,0);
	alignment = ALIGN_LEFT;
}

Vector3f RenderItem::getTranslation() {
	if (flags & RENDERITEM_FLAG_HAS_MATRIX) {
		return mat.getTranslation();
	} else {
		return pos;
	}
}
