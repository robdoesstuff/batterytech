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
	virtual void layout();
	virtual void setDrawableBounds(S32 left, S32 top, S32 right, S32 bottom);
	virtual void update();
	virtual void draw();

};

#endif /* BUTTON_H_ */
