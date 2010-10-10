/*
 * LinearLayoutParameters.h
 *
 *  Created on: Oct 7, 2010
 *      Author: rgreen
 */

#ifndef LINEARLAYOUTPARAMETERS_H_
#define LINEARLAYOUTPARAMETERS_H_

#include "LayoutParameters.h"

class LayoutParameters {
public:
	enum HorizontalAlignment { LEFT, HORIZONTAL_CENTER, RIGHT };
	enum VerticalAlignment { TOP, VERTICAL_CENTER, BOTTOM };
	LayoutParameters() {
		this->horizontalAlignment = LEFT;
		this->verticalAlignment = TOP;
	}
	LayoutParameters(HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment) {
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
	virtual ~LayoutParameters() {
	}
private:
	HorizontalAlignment horizontalAlignment;
	VerticalAlignment verticalAlignment;
};

#endif /* LINEARLAYOUTPARAMETERS_H_ */
