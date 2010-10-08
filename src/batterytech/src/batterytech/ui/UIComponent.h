/*
 * UIComponent.h
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#ifndef UICOMPONENT_H_
#define UICOMPONENT_H_

#include "../primitives.h"
#include "LayoutParameters.h"
#include "../logger.h"
#include <stdio.h>

#define FILL -1
#define WRAP 0

class UIComponent {
public:
	UIComponent();
	virtual ~UIComponent();
	// user methods, default unit is density-independent pixels
	virtual void setWidth(S32 widthDips) {this->widthDips = widthDips;};
	virtual void setHeight(S32 heightDips) {this->heightDips = heightDips;};
	virtual void setSize(S32 widthDips, S32 heightDips) {this->widthDips = widthDips; this->heightDips = heightDips;};
	virtual void setPadding(S32 paddingDips) {this->paddingDips = paddingDips;};
	virtual void setMargins(S32 marginDips) {this->marginLeftDips = this->marginTopDips = this->marginRightDips = this->marginBottomDips = marginDips;};
	virtual void setLayoutParameters(LayoutParameters *layoutParameters) {this->layoutParameters = layoutParameters;};

	// framework methods
	virtual S32 getDesiredWidth() {return widthDips;};
	virtual S32 getDesiredHeight() {return heightDips;};
	virtual LayoutParameters* getLayoutParameters() {return layoutParameters;};
	virtual void layout(F32 scale)=0;
	// in actual screen coordinates
	virtual void setDrawableBounds(S32 left, S32 top, S32 right, S32 bottom) {
		char buf[40];
		sprintf(buf, "Component Bounds set to [%d, %d, %d, %d]", left, top, right, bottom);
		log(buf);
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}
	virtual void update()=0;
	virtual void draw()=0;
	S32 paddingDips;
	S32 marginLeftDips, marginTopDips, marginRightDips, marginBottomDips;
protected:
	S32 widthDips;
	S32 heightDips;
	S32 left, top, right, bottom;
	LayoutParameters *layoutParameters;
};

#endif /* UICOMPONENT_H_ */
