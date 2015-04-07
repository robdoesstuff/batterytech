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
// Name        : Checkbox.cpp
// Description : A checkbox which can be checked or not checked.
//             : This is a compound component which consists of a button for the checkbox and a label for text.
//             : The text set for this component will show up on the right of the checkbox.
//============================================================================

#include "Checkbox.h"
#include "../Logger.h"
#include <stdio.h>

namespace BatteryTech {

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

	void Checkbox::onClickDown(S32 x, S32 y) {
		checked = !checked;
		if (checked) {
			button->setBackgroundMenuResource(checkedResourceId);
		} else {
			button->setBackgroundMenuResource(boxResourceId);
		}
	}

	void Checkbox::onClickUp(S32 x, S32 y) {

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

	void Checkbox::setTextColor(F32 r, F32 g, F32 b, F32 a) {
		label->setTextColor(r,g,b,a);
	}

}
