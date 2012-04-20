/*
 * ScreenControl.h
 *
 *  Created on: May 28, 2011
 *      Author: rgreen
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
