/*
 * GlobalLight.h
 *
 *  Created on: Oct 27, 2011
 *      Author: rgreen
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
    Vector4f shadowOrthoSize;
};

#endif /* GLOBALLIGHT_H_ */
