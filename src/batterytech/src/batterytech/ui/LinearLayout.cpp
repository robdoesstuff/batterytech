/*
 * LinearLayout.cpp
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#include "LinearLayout.h"
#include "../logger.h"
#include <stdio.h>
#include "LinearLayoutParameters.h"

LinearLayout::LinearLayout() : Layout() {
	this->layoutDirection = VERTICAL;
}

LinearLayout::LinearLayout(LayoutDirection direction) : Layout() {
	this->layoutDirection = direction;
}

S32 LinearLayout::getDesiredWidth() {
	if (widthDips == FILL) {
		return FILL;
	} else if (widthDips == WRAP) {
		// TODO - get wrap content width
		return WRAP;
	} else {
		return widthDips;
	}
}

S32 LinearLayout::getDesiredHeight() {
	if (heightDips == FILL) {
		return FILL;
	} else if (heightDips == WRAP) {
		// TODO - get wrap content width
		return WRAP;
	} else {
		return heightDips;
	}
}

LinearLayout::~LinearLayout() {
}

void LinearLayout::layout(F32 scale) {
	log("LinearLayout::layout starting");
	char buf[32];
	sprintf(buf, "Laying out %d UI Components", components->getSize());
	log(buf);
	S32 horizRemain = right - left;
	S32 vertRemain = bottom - top;
	S32 curLeft = left;
	S32 curRight = right;
	S32 curTop = top;
	S32 curBottom = bottom;
	S32 horizFillCount = 0;
	S32 vertFillCount = 0;
	S32 totalWidthNeeded = 0;
	S32 totalHeightNeeded = 0;
	S32 compLeft, compRight, compTop, compBottom;
	S32 i;
	log("calcing space dist 1");
	for (i = 0; i < components->getSize(); i++) {
		UIComponent *component = components->array[i];
		if (layoutDirection == VERTICAL) {
			if (component->getDesiredHeight() == FILL) {
				++vertFillCount;
			} else if (component->getDesiredHeight() == WRAP) {
				// TODO - handle wrap
			} else {
				totalHeightNeeded += (S32)(component->getDesiredHeight() * scale + component->marginTopDips * scale + component->marginBottomDips * scale);
			}
		} else {
			// horizontal
			if (component->getDesiredWidth() == FILL) {
				++horizFillCount;
			} else if (component->getDesiredWidth() == WRAP) {
				// TODO - handle wrap
			} else {
				totalWidthNeeded += (S32)(component->getDesiredWidth() * scale + component->marginLeftDips * scale + component->marginRightDips * scale);
			}
		}
	}
	log("calcing space dist 2");
	// this distributes remaining space to components which require fill
	S32 vertFillPerComponentSize = 0;
	S32 horizFillPerComponentSize = 0;
	if (layoutDirection == VERTICAL) {
		if (vertFillCount) {
			vertFillPerComponentSize = (vertRemain - totalHeightNeeded) / vertFillCount;
		}
		horizFillPerComponentSize = horizRemain;
	} else {
		//horizontal
		if (horizFillCount) {
			horizFillPerComponentSize = (horizRemain - totalWidthNeeded) / horizFillCount;
		}
		vertFillPerComponentSize = vertRemain;
	}
	// actually do the layout
	// take things with requested size and set their drawable bounds until we're out of space
	// do 2 passes to figure out stretching, first pass calcs how much space is available in each direction
	// second pass divides the space up and assigns the portion to each
	log("doing components");
	for (i = 0; i < components->getSize(); i++) {
		UIComponent *component = components->array[i];
		LinearLayoutParameters::HorizontalAlignment horizAlign = LinearLayoutParameters::LEFT;
		LinearLayoutParameters::VerticalAlignment vertAlign = LinearLayoutParameters::TOP;
		// check layout parameters
		if (component->getLayoutParameters()) {
			LinearLayoutParameters *linearParams = dynamic_cast<LinearLayoutParameters*>(component->getLayoutParameters());
			if (linearParams) {
				horizAlign = linearParams->getHorizontalAlignment();
				vertAlign = linearParams->getVerticalAlignment();
			}
		}
		// horizontal part
		if (component->getDesiredWidth() == FILL) {
			// filling horizontal space
			if (layoutDirection == VERTICAL) {
				// just use all horizontal space
				compLeft = curLeft + (S32)(component->marginLeftDips * scale);
				compRight = curRight - (S32)(component->marginRightDips * scale);
			} else {
				// use current marker with shared fill space
				compLeft = curLeft + (S32)(component->marginLeftDips * scale);
				compRight = curLeft + horizFillPerComponentSize - (S32)(component->marginRightDips * scale);
				curLeft += horizFillPerComponentSize;
			}
		} else if (component->getDesiredWidth() == WRAP) {
			// TODO - handle wrap
		} else {
			// fixed-width component
			if (layoutDirection == VERTICAL) {
				// TODO - check alignment, handle left (Default) center and right
				compLeft = curLeft + (S32)(component->marginLeftDips * scale);
				compRight = compLeft + (S32)(component->getDesiredWidth() * scale);
			} else {
				compLeft = curLeft + (S32)(component->marginLeftDips * scale);
				compRight = compLeft + (S32)(component->getDesiredWidth() * scale);
				curLeft = compRight + (S32)(component->marginRightDips * scale);
			}
		}
		// vertical part
		if (component->getDesiredHeight() == FILL) {
			// filling vertical space
			if (layoutDirection == VERTICAL) {
				compTop = curTop + (S32)(component->marginTopDips * scale);
				compBottom = curTop + vertFillPerComponentSize - (S32)(component->marginBottomDips * scale);
				curBottom += vertFillPerComponentSize;
			} else {
				compTop = curTop + (S32)(component->marginTopDips * scale);
				compBottom = curBottom - (S32)(component->marginBottomDips * scale);
			}
		} else if (component->getDesiredHeight() == WRAP) {
			// TODO - handle wrap
		} else {
			// fixed-height component
			if (layoutDirection == VERTICAL) {
				compTop = curTop + (S32)(component->marginTopDips * scale);
				compBottom = compTop + (S32)(component->getDesiredHeight() * scale);
				curTop += compBottom + (S32)(component->marginBottomDips * scale);
			} else {
				// TODO - check alignment, handle top (Default) center and bottom
				compTop = curTop + (S32)(component->marginTopDips * scale);
				compBottom = compTop + (S32)(component->getDesiredHeight() * scale);
			}
		}
		component->setDrawableBounds(compLeft, compTop, compRight, compBottom);
	}
}

void LinearLayout::update() {

}

void LinearLayout::draw() {

}
