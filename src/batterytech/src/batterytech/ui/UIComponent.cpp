/*
 * UIComponent.cpp
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#include "UIComponent.h"

UIComponent::UIComponent() {
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
	text = NULL;
	textHorizontalAlignment = HORIZONTAL_CENTER;
	textVerticalAlignment = VERTICAL_CENTER;
}

UIComponent::~UIComponent() {
	delete layoutParameters;
}
