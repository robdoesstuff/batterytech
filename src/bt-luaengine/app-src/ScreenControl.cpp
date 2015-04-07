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

#include "ScreenControl.h"
#include <batterytech/util/strx.h>

ScreenControl::ScreenControl(const char *name, const char *label) {
	this->name = strDuplicate(name);
	this->label = strDuplicate(label);
	textureAssetName = NULL;
	textureUVs = Vector4f(0,0,0,0);
	drawableBounds = Vector4f(0,0,0,0);
	pointerBounds = Vector4f(0,0,0,0);
	isInteractive = FALSE;
}

ScreenControl::~ScreenControl() {
	delete [] name;
	name = NULL;
	if (textureAssetName) {
		delete [] textureAssetName;
	}
	textureAssetName = NULL;
}

const char* ScreenControl::getName() {
	return name;
}

void ScreenControl::setName(char *name) {
	if (this->name) {
		delete [] this->name;
	}
	this->name = strDuplicate(name);
}

const char* ScreenControl::getTextureAssetName() {
	return textureAssetName;
}

void ScreenControl::setTextureAssetName(const char *textureAssetName) {
	if (this->textureAssetName) {
		delete [] this->textureAssetName;
	}
	if (textureAssetName) {
		this->textureAssetName = strDuplicate(textureAssetName);
	} else {
		this->textureAssetName = NULL;
	}
}

const char* ScreenControl::getLabel() {
	return this->label;
}

void ScreenControl::setLabel(const char *label) {
	if (this->label) {
		delete [] this->label;
	}
	this->label = strDuplicate(label);
}
