/*
 * LinearLayout.h
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#ifndef LINEARLAYOUT_H_
#define LINEARLAYOUT_H_

#include "Layout.h"

class LinearLayout: public Layout {
public:
	LinearLayout();
	virtual ~LinearLayout();
	virtual void layout();
	virtual void setDrawableBounds(S32 left, S32 top, S32 right, S32 bottom);
	virtual void update();
	virtual void draw();
};

#endif /* LINEARLAYOUT_H_ */
