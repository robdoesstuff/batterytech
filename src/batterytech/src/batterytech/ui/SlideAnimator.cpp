/*
 * SlideAnimator.cpp
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#include "SlideAnimator.h"
#include "../platform/platformgl.h"

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

void SlideAnimator::drawPreComponent(GraphicsConfiguration *gConfig) {
	glPushMatrix();
	if (component) {
		S32 width = component->right - component->left;
		S32 height = component->bottom - component->top;
		F32 amount = time / animationTime;
		if (side == BOTTOM) {
			if (type == IN) {
				glTranslatef(0, (1 - amount) * height, 0);
			} else {
				glTranslatef(0, amount * height, 0);
			}
		} else if (side == TOP) {
			if (type == IN) {
				glTranslatef(0, amount * -height, 0);
			} else {
				glTranslatef(0, (1 - amount) * -height, 0);
			}
		} else if (side == LEFT) {
			if (type == IN) {
				glTranslatef((1 - amount) * -width, 0, 0);
			} else {
				glTranslatef(amount * -width, 0, 0);
			}
		} else if (side == RIGHT) {
			if (type == IN) {
				glTranslatef((1 - amount) * width, 0, 0);
			} else {
				glTranslatef(amount * width, 0, 0);
			}
		}
	}
};

void SlideAnimator::drawPostComponent(GraphicsConfiguration *gConfig) {
	glPopMatrix();
};

