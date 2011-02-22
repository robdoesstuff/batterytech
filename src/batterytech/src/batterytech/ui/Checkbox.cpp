/*
 * Checkbox.cpp
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#include "Checkbox.h"
#include "../Logger.h"
#include <stdio.h>


Checkbox::Checkbox(const char *text) : LinearLayout() {
	this->layoutDirection = LinearLayout::HORIZONTAL;
	checked = FALSE;
	button = new Button();
	button->setSize(32, 32);
	button->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::VERTICAL_CENTER));
	label = new Label(text);
	label->setSize(FILL, 32);
	label->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::VERTICAL_CENTER));
	addComponent(button);
	addComponent(label);
	this->isClickable = TRUE;
	this->isSelectable = TRUE;
	// this layout should handle all clicks
	button->isClickable = FALSE;
	label->isClickable = FALSE;
}

Checkbox::~Checkbox() {
	delete button;
	delete label;
}

void Checkbox::setBoxResourceId(S32 resourceId) {
	this->boxResourceId = resourceId;
	if (!checked) {
		button->setBackgroundMenuResource(resourceId);
	}
}

void Checkbox::setCheckedResourceId(S32 resourceId) {
	this->checkedResourceId = resourceId;
	if (checked) {
		button->setBackgroundMenuResource(resourceId);
	}
}

void Checkbox::onClickDown() {
	checked = !checked;
	if (checked) {
		button->setBackgroundMenuResource(checkedResourceId);
	} else {
		button->setBackgroundMenuResource(boxResourceId);
	}
}

void Checkbox::onClickUp() {

}

void Checkbox::setText(const char *text) {
	label->setText(text);
}

void Checkbox::update(F32 delta) {
	UIComponent::update(delta);
}

void Checkbox::setChecked(BOOL32 checked) {
	this->checked = checked;
	if (checked) {
		button->setBackgroundMenuResource(checkedResourceId);
	} else {
		button->setBackgroundMenuResource(boxResourceId);
	}
}

BOOL32 Checkbox::isChecked() {
	return checked;
}

void Checkbox::setTextAlignment(HorizontalAlignment horizAlignment, VerticalAlignment vertAlignment) {
	label->setTextAlignment(horizAlignment, vertAlignment);
}
