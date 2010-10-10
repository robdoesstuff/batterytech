/*
 * MenuLayoutParameters.h
 *
 *  Created on: Oct 10, 2010
 *      Author: rgreen
 */

#ifndef MENULAYOUTPARAMETERS_H_
#define MENULAYOUTPARAMETERS_H_

#include "LayoutParameters.h"

class MenuLayoutParameters: public LayoutParameters {
public:
	enum HorizontalAlignment { LEFT, HORIZONTAL_CENTER, RIGHT };
	enum VerticalAlignment { TOP, VERTICAL_CENTER, BOTTOM };
	MenuLayoutParameters() : LayoutParameters() {
		this->horizontalAlignment = LEFT;
		this->verticalAlignment = TOP;
	}
	MenuLayoutParameters(HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment) : LayoutParameters() {
		this->horizontalAlignment = horizontalAlignment;
		this->verticalAlignment = verticalAlignment;
	}
	void setHorizontalAlignment(HorizontalAlignment horizontalAlignment) {
		this->horizontalAlignment = horizontalAlignment;
	}
	void setVerticalAlignment(VerticalAlignment verticalAlignment) {
		this->verticalAlignment = verticalAlignment;
	}
	HorizontalAlignment getHorizontalAlignment() {
		return this->horizontalAlignment;
	}
	VerticalAlignment getVerticalAlignment() {
		return this->verticalAlignment;
	}
	virtual ~MenuLayoutParameters() {
	}
private:
	HorizontalAlignment horizontalAlignment;
	VerticalAlignment verticalAlignment;
};

#endif /* MENULAYOUTPARAMETERS_H_ */
