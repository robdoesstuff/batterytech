/*
 * Button.cpp
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#include "Button.h"

Button::Button(const char *text) : UIComponent(text) {
}

Button::~Button() {
}

void Button::update(F32 delta) {
	UIComponent::update(delta);
}


