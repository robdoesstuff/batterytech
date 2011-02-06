/*
 * tvec2d.h
 *
 *  Created on: Nov 29, 2010
 *      Author: rgreen
 */

#ifndef TVEC2D_H_
#define TVEC2D_H_

struct TVEC2D {
	/// Default constructor does nothing (for performance).
	TVEC2D() {}

	/// Construct using coordinates.
	TVEC2D(float x, float y) : x(x), y(y) {}

	float x,y;
};

#endif /* TVEC2D_H_ */
