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
// Name        : SlideAnimator.cpp
// Description : Animator providing Slide-in/Slide-out effect in any direction on any side of screen
//============================================================================

#include "SlideAnimator.h"
#include "../platform/platformgl.h"
#include "../Context.h"
#include "../render/RenderContext.h"

namespace BatteryTech {

	SlideAnimator::SlideAnimator(SlideSide side, SlideType type, F32 time) {
		this->side = side;
		this->type = type;
		this->animationTime = time;
	}

	SlideAnimator::~SlideAnimator() {
	}

	void SlideAnimator::reset() {
		time = 0;
	}

	void SlideAnimator::update(F32 delta) {
		time += delta;
	}

	BOOL32 SlideAnimator::isComplete() {
		if (time > animationTime) {
			return TRUE;
		}
		return FALSE;
	}

	void SlideAnimator::drawPreComponent(Context *context) {
		if (context->gConfig->useShaders) {
			tempMatrix = context->renderContext->mvMatrix;
		} else {
			glPushMatrix();
		}
		if (component) {
			S32 width = component->right - component->left;
			S32 height = component->bottom - component->top;
			F32 amount = time / animationTime;
			F32 translateX = 0;
			F32 translateY = 0;
			if (side == BOTTOM) {
				if (type == SLIDE_IN) {
					translateY = (1 - amount) * height;
				} else {
					translateY = amount * height;
				}
			} else if (side == TOP) {
				if (type == SLIDE_IN) {
					translateY = (1 - amount) * -height;
				} else {
					translateY = amount * -height;
				}
			} else if (side == LEFT) {
				if (type == SLIDE_IN) {
					translateX = (1 - amount) * -width;
				} else {
					translateX = amount * -width;
				}
			} else if (side == RIGHT) {
				if (type == SLIDE_IN) {
					translateX = (1 - amount) * width;
				} else {
					translateX = amount * width;
				}
			}
			if (context->gConfig->useShaders) {
				context->renderContext->mvMatrix.translate(translateX, translateY, 0);
			} else {
				glTranslatef(translateX, translateY, 0);
			}
		}
	};

	void SlideAnimator::drawPostComponent(Context *context) {
		if (context->gConfig->useShaders) {
			context->renderContext->mvMatrix = tempMatrix;
		} else {
			glPopMatrix();
		}
	};

}
