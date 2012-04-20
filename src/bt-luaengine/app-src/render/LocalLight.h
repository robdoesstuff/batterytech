/*
 * LocalLight.h
 *
 *  Created on: Oct 27, 2011
 *      Author: rgreen
 */

#ifndef LOCALLIGHT_H_
#define LOCALLIGHT_H_

#include <batterytech/math/Vector3.h>
#include <batterytech/math/Vector4.h>
#include <batterytech/primitives.h>

using namespace BatteryTech;

class LocalLight {
public:
	LocalLight();
	virtual ~LocalLight();
	Vector3f position;
	Vector3f spotDirection;
	F32 spotExponent;
	F32 constantAttenuation;
	F32 linearAttenuation;
	F32 quadraticAttenuation;
	Vector4f ambient;
	Vector4f diffuse;
	Vector4f specular;
	void reset();
};

#endif /* LOCALLIGHT_H_ */
