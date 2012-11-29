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

#ifndef GLOBALLIGHT_H_
#define GLOBALLIGHT_H_

#include <batterytech/math/Vector2.h>
#include <batterytech/math/Vector3.h>
#include <batterytech/math/Vector4.h>
#include <batterytech/primitives.h>

using namespace BatteryTech;

class GlobalLight {
public:
	GlobalLight();
	virtual ~GlobalLight();
	// Direction TO light, not from light.
	Vector3f origin; // only used for shadow generation
	Vector3f direction;
	Vector4f ambient;
	Vector4f diffuse;
	Vector4f specular;
	Vector3f shadowColor;
	F32 shadowEpsilon;
	Vector2f shadowFrustumNearFar;
    BOOL32 shadowUsePerspective;
    F32 shadowPerspectiveFOV;
    S32 shadowCullMode;
    Vector4f shadowOrthoSize;
};

#endif /* GLOBALLIGHT_H_ */
