/*
 * UIComponent.cpp
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#include "UIComponent.h"

UIComponent::UIComponent(const char *text) {
	widthDips = 0;
	heightDips = 0;
	paddingLeftDips = paddingTopDips = paddingRightDips = paddingBottomDips = 0;
	marginBottomDips = 0;
	marginLeftDips = 0;
	marginRightDips = 0;
	marginTopDips = 0;
	left = 0;
	top = 0;
	right = 0;
	bottom = 0;
	backgroundMenuResourceId = NO_RESOURCE;
	pressedBackgroundMenuResourceId = NO_RESOURCE;
	selectedBackgroundMenuResourceId = NO_RESOURCE;
	this->text = text;
	textHorizontalAlignment = HORIZONTAL_CENTER;
	textVerticalAlignment = VERTICAL_CENTER;
	components = new ManagedArray<UIComponent> (10);
	isClickable = TRUE;
	isSelectable = TRUE;
	isEnabled = TRUE;
	isPressed = FALSE;
	isEntering = FALSE;
	isExiting = FALSE;
	removeFromView = FALSE;
	enterAnimator = NULL;
	mainAnimator = NULL;
	exitAnimator = NULL;
}

void UIComponent::addComponent(UIComponent *component) {
	components->add(component);
}

void UIComponent::dispatchClickDown() {
	onClickDown();
}

void UIComponent::dispatchClickUp() {
	onClickUp();
}

void UIComponent::update(F32 delta) {
	if (isEntering && enterAnimator) {
		enterAnimator->update(delta);
		if (enterAnimator->isComplete()) {
			isEntering = FALSE;
			if (mainAnimator) {
				mainAnimator->reset();
			}
		}
	} else if (mainAnimator && !isEntering && !isExiting) {
		mainAnimator->update(delta);
	} else if (isExiting && exitAnimator) {
		exitAnimator->update(delta);
		if (exitAnimator->isComplete()) {
			isExiting = FALSE;
			removeFromView = TRUE;
		}
	}
}

void UIComponent::enter() {
	removeFromView = FALSE;
	isExiting = FALSE;
	if (enterAnimator) {
		enterAnimator->reset();
		isEntering = TRUE;
	} else {
		isEntering = FALSE;
	}
	if (components) {
		S32 i;
		for (i = 0; i < components->getSize(); i++) {
			UIComponent *subComp = components->array[i];
			subComp->enter();
		}
	}

}

void UIComponent::exit() {
	isEntering = FALSE;
	if (exitAnimator) {
		exitAnimator->reset();
		isExiting = TRUE;
	} else {
		isExiting = FALSE;
		removeFromView = TRUE;
	}
	if (components) {
		S32 i;
		for (i = 0; i < components->getSize(); i++) {
			UIComponent *subComp = components->array[i];
			subComp->exit();
		}
	}
}

BOOL32 UIComponent::isExitDone() {
	if (removeFromView) {
		if (components) {
			S32 i;
			for (i = 0; i < components->getSize(); i++) {
				UIComponent *subComp = components->array[i];
				if (!subComp->isExitDone()) {
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

UIAnimator* UIComponent::getActiveAnimator() {
	if (isEntering && enterAnimator) {
		return enterAnimator;
	} else if ((isExiting || removeFromView) && exitAnimator) {
		return exitAnimator;
	} else if (mainAnimator) {
		return mainAnimator;
	}
	return NULL;
}

void UIComponent::setEnterAnimator(UIAnimator *animator) {
	this->enterAnimator = animator;
	if (animator) {
		animator->setComponent(this);
	}
}

void UIComponent::setMainAnimator(UIAnimator *animator) {
	this->mainAnimator = animator;
	if (animator) {
		animator->setComponent(this);
	}
}

void UIComponent::setExitAnimator(UIAnimator *animator) {
	this->exitAnimator = animator;
	if (animator) {
		animator->setComponent(this);
	}
}

UIComponent::~UIComponent() {
	delete enterAnimator;
	delete mainAnimator;
	delete exitAnimator;
	delete layoutParameters;
	delete components;
}
