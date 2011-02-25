/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : Vec2f.h
// Description : 2 value vector structure.  No math included for now.
//============================================================================

#ifndef VEC2F_H_
#define VEC2F_H_

struct Vec2f {
	/// Default constructor does nothing (for performance).
	Vec2f() {}

	/// Construct using coordinates.
	Vec2f(float x, float y) : x(x), y(y) {}

	float x,y;
};

#endif /* VEC2F_H_ */
