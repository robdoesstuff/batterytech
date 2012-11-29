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

#ifndef PHYSICSBODYOBJECT_H_
#define PHYSICSBODYOBJECT_H_

#include <batterytech/primitives.h>

#define PHYSICS_BODY_TYPE_GAMEOBJECT 0
#define PHYSICS_BODY_TYPE_STATIC_GEOMETRY 1

/**
 * This class is just to differentiate between gameobjects and static geometry in the box2d callbacks
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
