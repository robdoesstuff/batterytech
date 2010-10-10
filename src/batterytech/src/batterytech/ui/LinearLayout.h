/*
 * LinearLayout.h
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#ifndef LINEARLAYOUT_H_
#define LINEARLAYOUT_H_

#include "../primitives.h"
#include "Layout.h"

class LinearLayout: public Layout {
public:
	enum LayoutDirection { VERTICAL, HORIZONTAL };
	LinearLayout();
	LinearLayout(LayoutDirection direction);
	virtual ~LinearLayout();
	virtual void layout(F32 scale);
	virtual void update();
	virtual void draw();
	virtual S32 getDesiredWidth();
	virtual S32 getDesiredHeight();
private:
	void calcSpaceRequired(F32 scale, S32 *width, S32 *height, S32 *horizFillCount, S32 *vertFillCount, S32 *center);
	LayoutDirection layoutDirection;
};

#endif /* LINEARLAYOUT_H_ */
