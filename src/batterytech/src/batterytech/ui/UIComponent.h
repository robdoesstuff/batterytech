/*
 * UIComponent.h
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#ifndef UICOMPONENT_H_
#define UICOMPONENT_H_

#include "../primitives.h"

class UIComponent {
public:
	UIComponent();
	virtual ~UIComponent();
	// user methods, default unit is density-independent pixels
	virtual void setWidth(S32 widthDips) {this->widthDips = widthDips;};
	virtual void setHeight(S32 heightDips) {this->heightDips = heightDips;};
	virtual void setPadding(S32 paddingDips) {this->paddingDips = paddingDips;};
	virtual void setMargins(S32 marginDips) {this->marginDips = marginDips;};

	// framework methods
	virtual S32 getDesiredWidth() {return widthDips;};
	virtual S32 getDesiredHeight() {return heightDips;};
	virtual void layout()=0;
	// in actual screen coordinates
	virtual void setDrawableBounds(S32 left, S32 top, S32 right, S32 bottom)=0;
	virtual void update()=0;
	virtual void draw()=0;
protected:
	S32 widthDips;
	S32 heightDips;
	S32 paddingDips;
	S32 marginDips;
};

#endif /* UICOMPONENT_H_ */
