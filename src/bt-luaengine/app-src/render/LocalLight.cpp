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

#include "LocalLight.h"

LocalLight::LocalLight() {
	reset();
}

LocalLight::~LocalLight() {
}

void LocalLight::reset() {
	position = Vector3f(0,0,0);
	spotDirection = Vector3f(0,0,0);
	spotExponent = 0;
	constantAttenuation = 0;
	linearAttenuation = 0;
	quadraticAttenuation = 0;
	ambient = Vector4f(0,0,0,0);
	diffuse = Vector4f(0,0,0,0);
	specular = Vector4f(0,0,0,0);
}
