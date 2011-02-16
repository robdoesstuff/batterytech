/*
 * SlideAnimator.cpp
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#include "SlideAnimator.h"
#include "../platform/platformgl.h"
#include "../Context.h"
#include "../render/RenderContext.h"

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
		esCopy(&tempMatrix, &context->renderContext->mvMatrix);
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
				translateY = amount * -height;
			} else {
				translateY = (1 - amount) * -height;
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
			esTranslate(&context->renderContext->mvMatrix, translateX, translateY, 0);
		} else {
			glTranslatef(translateX, translateY, 0);
		}
	}
};

void SlideAnimator::drawPostComponent(Context *context) {
	if (context->gConfig->useShaders) {
		esCopy(&context->renderContext->mvMatrix, &tempMatrix);
	} else {
		glPopMatrix();
	}
};

