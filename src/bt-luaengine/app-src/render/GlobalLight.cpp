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
    shadowUsePerspective = FALSE;
    shadowPerspectiveFOV = 45;
    shadowOrthoSize = Vector4f(100,100,100,100);
    shadowCullMode = 1;
}

GlobalLight::~GlobalLight() {
}
