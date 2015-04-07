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

#ifndef SCREENCONTROL_H_
#define SCREENCONTROL_H_

#include <batterytech/primitives.h>
#include <batterytech/math/Vector4.h>

using namespace BatteryTech;

class ScreenControl {
public:
	ScreenControl(const char *name, const char *label);
	virtual ~ScreenControl();
	const char* getName();
	void setName(char *name);
	const char* getTextureAssetName();
	void setTextureAssetName(const char *textureAssetName);
	const char* getLabel();
	void setLabel(const char *label);
	Vector4f textureUVs;
	Vector4f pointerBounds;
	Vector4f drawableBounds;
	BOOL32 isInteractive;
protected:
	char *name;
	char *textureAssetName;
	char *label;
};

#endif /* SCREENCONTROL_H_ */
