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
// Name        : Button.cpp
// Description : A clickable button.
//============================================================================

#include "Button.h"

namespace BatteryTech {

	Button::Button(const char *text) : UIComponent(text) {
	}

	Button::~Button() {
	}

	void Button::update(F32 delta) {
		UIComponent::update(delta);
	}

}

