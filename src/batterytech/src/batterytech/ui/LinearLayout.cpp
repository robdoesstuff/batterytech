/*
 * LinearLayout.cpp
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#include "LinearLayout.h"
#include "../Logger.h"
#include <stdio.h>
#include "LayoutParameters.h"

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
		S32 widthNeeded = 0;
		S32 heightNeeded = 0;
		S32 vertFillCount = 0;
		S32 horizFillCount = 0;
		S32 centerNeeded = 0;
		// scale at 1.0 for dips
		calcSpaceRequired(1.0f, &widthNeeded, &heightNeeded, &horizFillCount, &vertFillCount, &centerNeeded);
		return widthNeeded + paddingLeftDips + paddingRightDips;
	} else {
		return widthDips;
	}
}

S32 LinearLayout::getDesiredHeight() {
	if (heightDips == FILL) {
		return FILL;
	} else if (heightDips == WRAP) {
		S32 widthNeeded = 0;
		S32 heightNeeded = 0;
		S32 vertFillCount = 0;
		S32 horizFillCount = 0;
		S32 centerNeeded = 0;
		// scale at 1.0 for dips
		calcSpaceRequired(1.0f, &widthNeeded, &heightNeeded, &horizFillCount, &vertFillCount, &centerNeeded);
		return heightNeeded + paddingTopDips + paddingBottomDips;
	} else {
		return heightDips;
	}
}

LinearLayout::~LinearLayout() {
}

void LinearLayout::layout(F32 scale) {
	logmsg("LinearLayout::layout starting");
	char buf[70];
	sprintf(buf, "Laying out %d UI Components at scale %g", components->getSize(), scale);
	logmsg(buf);
	S32 curLeft = (S32)(left + paddingLeftDips * scale);
	S32 curRight = (S32)(right - paddingRightDips * scale);
	S32 curTop = (S32)(top + paddingTopDips * scale);
	S32 curBottom = (S32)(bottom - paddingBottomDips * scale);
	S32 horizRemain = curRight - curLeft;
	S32 vertRemain = curBottom - curTop;
	S32 horizFillCount = 0;
	S32 vertFillCount = 0;
	S32 totalWidthNeeded = 0;
	S32 totalHeightNeeded = 0;
	S32 centerNeeded = 0;
	S32 centerUsed = 0;
	S32 compLeft, compRight, compTop, compBottom;
	S32 i;
	calcSpaceRequired(scale, &totalWidthNeeded, &totalHeightNeeded, &horizFillCount, &vertFillCount, &centerNeeded);
	logmsg("calcing space dist 2");
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
	logmsg("doing components");
	for (i = 0; i < components->getSize(); i++) {
		UIComponent *component = components->array[i];
		LayoutParameters::HorizontalAlignment horizAlign = LayoutParameters::LEFT;
		LayoutParameters::VerticalAlignment vertAlign = LayoutParameters::TOP;
		// check layout parameters
		if (component->getLayoutParameters()) {
			horizAlign = component->getLayoutParameters()->getHorizontalAlignment();
			vertAlign = component->getLayoutParameters()->getVerticalAlignment();
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
			// Is there anything to do about a wrap result?
		} else {
			// fixed-width component
			if (layoutDirection == VERTICAL) {
				if (horizAlign == LayoutParameters::LEFT) {
					compLeft = curLeft + (S32)(component->marginLeftDips * scale);
					compRight = compLeft + (S32)(component->getDesiredWidth() * scale);
				} if (horizAlign == LayoutParameters::HORIZONTAL_CENTER) {
					// use the difference of the left and right margins
					compLeft = (right - left) / 2 - (S32)(component->getDesiredWidth() * scale) / 2 + (S32)(component->marginLeftDips * scale - component->marginRightDips * scale);
					compRight = compLeft + (S32)(component->getDesiredWidth() * scale);
				} else if (horizAlign == LayoutParameters::RIGHT) {
					compRight = right - (S32)(component->marginRightDips * scale);
					compLeft = compRight - (S32)(component->getDesiredWidth() * scale);
				}
			} else {
				// horizontal layout
				if (horizAlign == LayoutParameters::LEFT) {
					compLeft = curLeft + (S32)(component->marginLeftDips * scale);
					compRight = compLeft + (S32)(component->getDesiredWidth() * scale);
					curLeft = compRight + (S32)(component->marginRightDips * scale);
				} if (horizAlign == LayoutParameters::HORIZONTAL_CENTER) {
					compLeft = (right - left) / 2 + - centerNeeded / 2 + centerUsed + (S32)(component->marginLeftDips * scale);
					compRight = compLeft + (S32)(component->getDesiredWidth() * scale);
					curLeft = compRight + (S32)(component->marginRightDips * scale);
					centerUsed += (S32)(component->getDesiredWidth() * scale) + (S32)(component->marginLeftDips * scale) + (S32)(component->marginRightDips * scale);
				} else if (horizAlign == LayoutParameters::RIGHT) {
					compRight = curRight - (S32)(component->marginRightDips * scale);
					compLeft = compRight - (S32)(component->getDesiredWidth() * scale);
					curRight = compLeft - (S32)(component->marginLeftDips * scale);
				}
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
			// Is there anything to do about a wrap result?
		} else {
			// fixed-height component
			if (layoutDirection == VERTICAL) {
				if (vertAlign == LayoutParameters::TOP) {
					compTop = curTop + (S32)(component->marginTopDips * scale);
					compBottom = compTop + (S32)(component->getDesiredHeight() * scale);
					curTop = compBottom + (S32)(component->marginBottomDips * scale);
				} else if (vertAlign == LayoutParameters::VERTICAL_CENTER) {
					compTop = (bottom - top) / 2 - (centerNeeded / 2) + centerUsed + (S32)(component->marginTopDips * scale);
					S32 compTotalVertSize = (S32)((component->getDesiredHeight() + component->marginTopDips + component->marginBottomDips) * scale);
					compBottom = compTop + (S32)(component->getDesiredHeight() * scale);
					centerUsed += compTotalVertSize;
				} else if (vertAlign == LayoutParameters::BOTTOM) {
					compBottom = curBottom - (S32)(component->marginBottomDips * scale);
					compTop = compBottom - (S32)(component->getDesiredHeight() * scale);
					curBottom = compTop - (S32)(component->marginTopDips * scale);
				}
			} else {
				// horizontal layout
				if (vertAlign == LayoutParameters::TOP) {
					compTop = curTop + (S32)(component->marginTopDips * scale);
					compBottom = compTop + (S32)(component->getDesiredHeight() * scale);
				} else if (vertAlign == LayoutParameters::VERTICAL_CENTER) {
					// use the difference of the top and bottom margins
					compTop = (bottom - top) / 2 - (S32)(component->getDesiredHeight() * scale) / 2 + (S32)(component->marginTopDips * scale  - component->marginBottomDips * scale);
					compBottom = compTop + (S32)(component->getDesiredHeight() * scale);
				} else if (vertAlign == LayoutParameters::BOTTOM) {
					compBottom = bottom - (S32)(component->marginBottomDips * scale);
					compTop = compBottom - (S32)(component->getDesiredHeight() * scale);
				}
			}
		}
		component->setDrawableBounds(compLeft, compTop, compRight, compBottom);
		component->layout(scale);
	}
}

void LinearLayout::calcSpaceRequired(F32 scale, S32 *width, S32 *height, S32 *horizFillCount, S32 *vertFillCount, S32 *center) {
	S32 i;
	for (i = 0; i < components->getSize(); i++) {
		UIComponent *component = components->array[i];
		LayoutParameters::HorizontalAlignment horizAlign = LayoutParameters::LEFT;
		LayoutParameters::VerticalAlignment vertAlign = LayoutParameters::TOP;
		// check layout parameters
		if (component->getLayoutParameters()) {
			horizAlign = component->getLayoutParameters()->getHorizontalAlignment();
			vertAlign = component->getLayoutParameters()->getVerticalAlignment();
		}
		if (layoutDirection == VERTICAL) {
			if (component->getDesiredHeight() == FILL) {
				++*vertFillCount;
			} else if (component->getDesiredHeight() == WRAP) {
				// Is there anything to do about a wrap result?
			} else {
				*height += (S32)(component->getDesiredHeight() * scale + component->marginTopDips * scale + component->marginBottomDips * scale);
				if (vertAlign == LayoutParameters::VERTICAL_CENTER) {
					*center += (S32)(component->getDesiredHeight() * scale + component->marginTopDips * scale + component->marginBottomDips * scale);
				}
				// just return the widest fixed-width component
				if (component->getDesiredWidth() != FILL && component->getDesiredWidth() != WRAP) {
					S32 componentTotalWidthPx = (S32)((component->getDesiredWidth() + component->marginLeftDips + component->marginRightDips) * scale);
					if (componentTotalWidthPx > *width) {
						*width = componentTotalWidthPx;
					}
				}
			}
		} else {
			// horizontal
			if (component->getDesiredWidth() == FILL) {
				++*horizFillCount;
			} else if (component->getDesiredWidth() == WRAP) {
				// Is there anything to do about a wrap result?
			} else {
				*width += (S32)(component->getDesiredWidth() * scale + component->marginLeftDips * scale + component->marginRightDips * scale);
				if (horizAlign == LayoutParameters::HORIZONTAL_CENTER) {
					*center += (S32)(component->getDesiredWidth() * scale + component->marginLeftDips * scale + component->marginRightDips * scale);
				}
				// just return the tallest fixed-height component
				if (component->getDesiredHeight() != FILL && component->getDesiredHeight() != WRAP) {
					S32 componentTotalHeightPx = (S32)((component->getDesiredHeight() + component->marginTopDips + component->marginBottomDips) * scale);
					if (componentTotalHeightPx > *height) {
						*height = componentTotalHeightPx;
					}
				}
			}
		}
	}
}

void LinearLayout::update(F32 delta) {

}

