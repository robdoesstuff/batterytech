/*
 * LocalLight.cpp
 *
 *  Created on: Oct 27, 2011
 *      Author: rgreen
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
