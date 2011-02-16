/*
 * SlideAnimator.h
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#ifndef SLIDEANIMATOR_H_
#define SLIDEANIMATOR_H_

#include "UIAnimator.h"
#include "../primitives.h"
#include "../util/esTransform.h"

class Context;

class SlideAnimator : public UIAnimator {
public:
	enum SlideSide { LEFT, TOP, RIGHT, BOTTOM };
	enum SlideType { SLIDE_IN, SLIDE_OUT };
	SlideAnimator(SlideSide side, SlideType type, F32 time);
	virtual ~SlideAnimator();
	virtual void reset();
	virtual void update(F32 delta);
	virtual void drawPreComponent(Context *context);
	virtual void drawPostComponent(Context *context);
	virtual BOOL32 isComplete();
private:
	F32 time;
	F32 animationTime;
	SlideSide side;
	SlideType type;
	ESMatrix tempMatrix;
};

#endif /* SLIDEANIMATOR_H_ */
