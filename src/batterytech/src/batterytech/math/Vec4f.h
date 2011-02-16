/*
 * Vec4f.h
 *
 *  Created on: Feb 16, 2011
 *      Author: rgreen
 */

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
