/*
 * PhysicsBodyObject.h
 *
 *  Created on: Dec 3, 2010
 *      Author: rgreen
 */

#ifndef PHYSICSBODYOBJECT_H_
#define PHYSICSBODYOBJECT_H_

#include <batterytech/primitives.h>

#define PHYSICS_BODY_TYPE_GAMEOBJECT 0
#define PHYSICS_BODY_TYPE_CONTOUR 1

/**
 * This class is just to differentiate between gameobjects and level contours in the box2d callbacks
 *
 * Classes that extend this are what will be called back
 */
class PhysicsBodyObject {
public:
	PhysicsBodyObject(S32 bodyType) { this->bodyType = bodyType; }
	virtual ~PhysicsBodyObject() {};
	S32 bodyType;
};

#endif /* PHYSICSBODYOBJECT_H_ */
