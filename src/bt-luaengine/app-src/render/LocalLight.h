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
