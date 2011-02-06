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

class SlideAnimator : public UIAnimator {
public:
	enum SlideSide { LEFT, TOP, RIGHT, BOTTOM };
	enum SlideType { SLIDE_IN, SLIDE_OUT };
	SlideAnimator(SlideSide side, SlideType type, F32 time);
	virtual ~SlideAnimator();
	virtual void reset();
	virtual void update(F32 delta);
	virtual void drawPreComponent(GraphicsConfiguration *gConfig);
	virtual void drawPostComponent(GraphicsConfiguration *gConfig);
	virtual BOOL32 isComplete();
private:
	F32 time;
	F32 animationTime;
	SlideSide side;
	SlideType type;
};

#endif /* SLIDEANIMATOR_H_ */
