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
// Name        : Vec4f.h
// Description : 4 value vector structure.  No math included for now.
//============================================================================

#ifndef VEC4F_H_
#define VEC4F_H_

#include "../primitives.h"

struct Vec4f {
	/// Default constructor does nothing (for performance).
	Vec4f() {}

	/// Construct using coordinates.
	Vec4f(F32 x, F32 y, F32 z, F32 a) : x(x), y(y), z(z), a(a) {}

	F32 x,y,z,a;
};


#endif /* VEC4F_H_ */
