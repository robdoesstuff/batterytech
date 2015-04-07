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

#ifndef CAMERA_H_
#define CAMERA_H_

#include <batterytech/Context.h>
#include <batterytech/math/Vector3.h>
#include <batterytech/math/Matrix4.h>
#include <batterytech/primitives.h>

using namespace BatteryTech;

class Camera {
public:
	Camera(Context *context, Vector3f pos, F32 tilt, F32 rotation);
	virtual ~Camera();
	void update();
	void zoom(F32 amount);
	void pan(F32 xAmount, F32 yAmount);
	void setNearFarFOV(F32 near, F32 far, F32 fov);
	Vector3f castRayFromViewport(F32 viewX, F32 viewY);
	BOOL32 isInView(Vector3f aabbMin, Vector3f aabbMax);
	Context *context;
	Vector3f pos;
	Vector3f look;
	Vector3f up;
	Vector3f right;
	F32 tilt;
	F32 rotation;
	// matrix to use for MV
	Matrix4f matrix;
	// matrix to use for the projection
	Matrix4f proj;
	// inverse rotation matrix to use to project points from camera, calculate look, up, right, etc
	Matrix3f invRotMatrix;
private:
	Vector3f frustumAABBMin, frustumAABBMax;
	F32 nearClip, farClip, fov;
	F32 nearWidth, nearHeight, farWidth, farHeight;
};

#endif /* CAMERA_H_ */
