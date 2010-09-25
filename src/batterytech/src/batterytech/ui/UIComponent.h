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
	virtual void setWidth(S32 widthDips)=0;
	virtual void setHeight(S32 heightDips)=0;
	virtual void setPadding(S32 paddingDips)=0;
	virtual void setMargins(S32 marginDips)=0;

	// framework methods
	virtual void layout()=0;
	virtual S32 getDesiredWidth()=0;
	virtual S32 getDesiredHeight()=0;
	virtual void setDrawableBounds()=0;
	virtual void update()=0;
	virtual void draw()=0;
};

#endif /* UICOMPONENT_H_ */
