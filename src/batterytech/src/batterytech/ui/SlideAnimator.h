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

//============================================================================
// Name        : SlideAnimator.h
// Description : Animator providing Slide-in/Slide-out effect in any direction on any side of screen
//============================================================================

#ifndef SLIDEANIMATOR_H_
#define SLIDEANIMATOR_H_

#include "UIAnimator.h"
#include "../primitives.h"
#include "../math/Matrix4.h"

namespace BatteryTech {

	class Context;

	/** \brief SlideAnimator Adds slide-in and slide-out effects to UIComponents
	 * \ingroup UI
	 * \class SlideAnimator
	 */
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
		Matrix4f tempMatrix;
	};

}

#endif /* SLIDEANIMATOR_H_ */
