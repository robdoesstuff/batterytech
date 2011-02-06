/*
 * Triangulator.h
 *
 *  Created on: Oct 31, 2010
 *      Author: rgreen
 */

#ifndef TRIANGULATOR_H_
#define TRIANGULATOR_H_

#include "tvec2d.h"

class Triangulator
{
public:

  // triangulate a contour/polygon, make sure result is at least (contourSize - 2) * 3 in size!
  static bool Process(TVEC2D *contour, int contourSize,
		  TVEC2D *result, int *resultSize);

  // compute area of a contour/polygon
  static float Area(TVEC2D *contour, int contourSize);

  // decide if point Px/Py is inside triangle defined by
  // (Ax,Ay) (Bx,By) (Cx,Cy)
  static bool InsideTriangle(float Ax, float Ay,
                      float Bx, float By,
                      float Cx, float Cy,
                      float Px, float Py);


private:
  static bool Snip(TVEC2D *contour, int contourSize, int u,int v,int w,int n,int *V);

};

#endif /* TRIANGULATOR_H_ */
