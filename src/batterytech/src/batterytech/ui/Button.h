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
	Button();
	virtual ~Button();
	virtual void update();
	virtual void draw();
	virtual void layout(F32 scale);

};

#endif /* BUTTON_H_ */
