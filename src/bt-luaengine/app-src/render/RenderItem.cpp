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
	animator = NULL;
	maxPointLights = 0;
	flags = 0;
	colorFilter = Vector4f(1,1,1,1);
	viewport = Vector4i(0,0,0,0);
	alignment = ALIGN_LEFT;
    sortValue = 0;
}

Vector3f RenderItem::getTranslation() {
	if (flags & RENDERITEM_FLAG_HAS_MATRIX) {
		return mat.getTranslation();
	} else {
		return pos;
	}
}
