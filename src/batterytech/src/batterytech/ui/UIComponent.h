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
#include "../Logger.h"
#include <stdio.h>
#include "../util/ManagedArray.h"

#define FILL -1
#define WRAP 0
#define NO_RESOURCE -1

class UIComponent {
public:
	enum HorizontalAlignment { LEFT, HORIZONTAL_CENTER, RIGHT };
	enum VerticalAlignment { TOP, VERTICAL_CENTER, BOTTOM };
	UIComponent();
	virtual ~UIComponent();
	// user methods, default unit is density-independent pixels
	virtual void setWidth(S32 widthDips) {this->widthDips = widthDips;};
	virtual void setHeight(S32 heightDips) {this->heightDips = heightDips;};
	virtual void setSize(S32 widthDips, S32 heightDips) {this->widthDips = widthDips; this->heightDips = heightDips;};
	virtual void setPadding(S32 paddingDips) {this->paddingLeftDips = this->paddingTopDips = this->paddingRightDips = this->paddingBottomDips = paddingDips;};
	virtual void setMargins(S32 marginDips) {this->marginLeftDips = this->marginTopDips = this->marginRightDips = this->marginBottomDips = marginDips;};
	virtual void setLayoutParameters(LayoutParameters *layoutParameters) {this->layoutParameters = layoutParameters;};
	virtual void setBackgroundMenuResource(S32 menuResourceId) {this->backgroundMenuResourceId = menuResourceId;};
	virtual void setText(const char *text) {this->text = text;};
	virtual void setTextAlignment(HorizontalAlignment horizAlignment, VerticalAlignment vertAlignment) {
		this->textHorizontalAlignment = horizAlignment;
		this->textVerticalAlignment = vertAlignment;
	}
	virtual void addComponent(UIComponent *component);
	// framework methods
	virtual S32 getDesiredWidth() {return widthDips;};
	virtual S32 getDesiredHeight() {return heightDips;};
	virtual LayoutParameters* getLayoutParameters() {return layoutParameters;};
	virtual void layout(F32 scale)=0;
	// in actual screen coordinates
	virtual void setDrawableBounds(S32 left, S32 top, S32 right, S32 bottom) {
		char buf[60];
		sprintf(buf, "Component Bounds set to [%d, %d, %d, %d]", left, top, right, bottom);
		log(buf);
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}
	virtual void update()=0;
	virtual void draw()=0;
	S32 paddingLeftDips, paddingTopDips, paddingRightDips, paddingBottomDips;
	S32 marginLeftDips, marginTopDips, marginRightDips, marginBottomDips;
	S32 backgroundMenuResourceId;
	const char *text;
	S32 left, top, right, bottom;
	HorizontalAlignment textHorizontalAlignment;
	VerticalAlignment textVerticalAlignment;
	ManagedArray<UIComponent> *components;
protected:
	S32 widthDips;
	S32 heightDips;
	LayoutParameters *layoutParameters;
};

#endif /* UICOMPONENT_H_ */
