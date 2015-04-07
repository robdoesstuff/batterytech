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

#include "Camera.h"
#include <batterytech/render/GraphicsConfiguration.h>
#include <batterytech/platform/opengles.h>
#include <batterytech/Logger.h>
#include <stdio.h>

Camera::Camera(Context *context, Vector3f pos, F32 tilt, F32 rotation) {
	this->context = context;
	this->pos = pos;
	this->tilt = tilt;
	this->rotation = rotation;
	setNearFarFOV(2.0f, 600.0f, 60.0f);
	update();
}

Camera::~Camera() {
}

void Camera::setNearFarFOV(F32 near, F32 far, F32 fov) {
	this->nearClip = near;
	this->farClip = far;
	this->fov = fov;
	proj.identity();
	F32 aspect = context->gConfig->width / (F32)context->gConfig->height;
	proj.perspective(fov, aspect, near, far);
	F32 fovCalc = 2 * tan((fov * (PI / 180.0f)) / 2);
	nearHeight = fovCalc * near;
	nearWidth = nearHeight * aspect;
	farHeight = fovCalc * far;
	farWidth = farHeight * aspect;
}

void Camera::update() {
	matrix.identity();
	matrix.rotate(tilt, 1.0f, 0, 0);
	matrix.rotate(rotation, 0, 0, 1.0f);
	matrix.translate(-pos.x, -pos.y, -pos.z);
	matrix.toMatrix3(invRotMatrix);
	invRotMatrix = invRotMatrix.inverse();
	look = invRotMatrix * Vector3f(0, 0, -1.0f);
	look.normalize();
	up = invRotMatrix * Vector3f(0, 1.0f, 0);
	up.normalize();
	right = invRotMatrix * Vector3f(1.0f, 0, 0);
	right.normalize();
	// calculate bounds of frustum
	// from position
	Vector3f planePoints[8];
	Vector3f *farPlane = &planePoints[0];
	Vector3f *nearPlane = &planePoints[4];
	Vector3f nearCenter = pos + look * nearClip;
	Vector3f farCenter = pos + look * farClip;
	//char buf[1024];
	//sprintf(buf, "nc=%f %f %f fc=%f %f %f", nearCenter.x, nearCenter.y, nearCenter.z, farCenter.x, farCenter.y, farCenter.z);
	//logmsg(buf);
	// top left, top right, bottom right, bottom left
	nearPlane[0] = nearCenter - right * nearWidth / 2 + up * nearHeight / 2;
	nearPlane[1] = nearPlane[0] + right * nearWidth;
	nearPlane[2] = nearPlane[1] - up * nearHeight;
	nearPlane[3] = nearPlane[0] - up * nearHeight;
	farPlane[0] = farCenter - right * farWidth / 2 + up * farHeight / 2;
	farPlane[1] = farPlane[0] + right * farWidth;
	farPlane[2] = farPlane[1] - up * farHeight;
	farPlane[3] = farPlane[0] - up * farHeight;
	frustumAABBMin = nearPlane[0];
	frustumAABBMax = nearPlane[0];
	for (S32 i = 0; i < 8; i++) {
		if (planePoints[i].x < frustumAABBMin.x) {
			frustumAABBMin.x = planePoints[i].x;
		}
		if (planePoints[i].y < frustumAABBMin.y) {
			frustumAABBMin.y = planePoints[i].y;
		}
		if (planePoints[i].z < frustumAABBMin.z) {
			frustumAABBMin.z = planePoints[i].z;
		}
		if (planePoints[i].x > frustumAABBMax.x) {
			frustumAABBMax.x = planePoints[i].x;
		}
		if (planePoints[i].y > frustumAABBMax.y) {
			frustumAABBMax.y = planePoints[i].y;
		}
		if (planePoints[i].z > frustumAABBMax.z) {
			frustumAABBMax.z = planePoints[i].z;
		}
	}
}

void Camera::zoom(F32 amt) {
	pos += look * amt;
}

void Camera::pan(F32 xAmt, F32 yAmt) {
	Vector3f left = invRotMatrix * Vector3f(-1,0,0);
	Vector3f forwardAxis = invRotMatrix * Vector3f(0,1,0);
	pos += left * xAmt;
	pos += forwardAxis * yAmt;
}

Vector3f Camera::castRayFromViewport(F32 viewX, F32 viewY) {
	Vector3f ray = gles_gluUnProject(Vector3f(viewX, context->gConfig->viewportHeight - viewY, 0.25f), matrix, proj,
			Vector2f(0,0), Vector2f(context->gConfig->viewportWidth, context->gConfig->viewportHeight));
	// compensation for a little floating point rounding error in all the projection math
	ray = ray * 1.0025f;
	ray = ray - pos;
	ray.normalize();
	return ray;
}

BOOL32 Camera::isInView(Vector3f aabbMin, Vector3f aabbMax) {
	// we're just doing the cheap AABB test instead of full point-in-planes test of frustum.
	// this will result in some false positives but is way cheaper.
	if (aabbMax.x < frustumAABBMin.x || aabbMax.y < frustumAABBMin.y || aabbMax.z < frustumAABBMin.z
			|| aabbMin.x > frustumAABBMax.x || aabbMin.y > frustumAABBMax.y || aabbMin.z > frustumAABBMax.z) {
		return FALSE;
	}
	return TRUE;
}
