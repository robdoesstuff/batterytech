/*
 * Button.h
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "UIComponent.h"

class Button: public UIComponent {
public:
	Button(const char *text = NULL);
	virtual ~Button();
	virtual void update(F32 delta);
};

#endif /* BUTTON_H_ */
