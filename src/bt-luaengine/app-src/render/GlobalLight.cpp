/*
 * GlobalLight.cpp
 *
 *  Created on: Oct 27, 2011
 *      Author: rgreen
 */

#include "GlobalLight.h"

GlobalLight::GlobalLight() {
	//defaults
	direction = Vector3f(0,0,1);
	ambient = Vector4f(.7,.7,.7,1);
	diffuse = Vector4f(.7,.7,.7,1);
	specular = Vector4f(.5,.5,.5,1);
	shadowColor = Vector3f(0.5, 0.5, 0.5);
	shadowEpsilon = 0.0005f;
	shadowFrustumNearFar = Vector2f(10.0f, 100.0f);
}

GlobalLight::~GlobalLight() {
}
