/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : LinearLayout.cpp
// Description : A layout that places components from left to right (horizontal) or top to bottom (vertical)
// Usage       : Every component in this layout must have LayoutParameters added to it.
//             : Components will be placed depending on the size and values of the layout parameters.
//============================================================================

#include "LinearLayout.h"
#include "../Logger.h"
#include <stdio.h>
#include "LayoutParameters.h"
#include "../Context.h"
#include "../render/MenuRenderer.h"

namespace BatteryTech {

	LinearLayout::LinearLayout(const char *text) : Layout(text) {
		this->layoutDirection = VERTICAL;
	}

	LinearLayout::LinearLayout(LayoutDirection direction) : Layout() {
		this->layoutDirection = direction;
	}

	S32 LinearLayout::getDesiredWidth(Context *context, S32 widthAvailable, S32 heightAvailable) {
		if (widthDips == FILL) {
			return FILL;
		} else if (widthDips == WRAP) {
			S32 widthNeeded = 0;
			S32 heightNeeded = 0;
			S32 vertFillCount = 0;
			S32 horizFillCount = 0;
			S32 centerNeeded = 0;
			// scale at 1.0 for dips
			S32 heightAssumption = heightAvailable;
			if (heightDips != FILL && heightDips != WRAP) {
				heightAssumption = heightDips;
			}
			calcSpaceRequired(context, 1.0f, 1.0f/context->gConfig->uiScale, &widthNeeded, &heightNeeded, &horizFillCount, &vertFillCount, &centerNeeded, widthAvailable, heightAssumption);
			return widthNeeded + paddingLeftDips + paddingRightDips;
		} else {
			return widthDips;
		}
	}

	S32 LinearLayout::getDesiredHeight(Context *context, S32 widthAvailable, S32 heightAvailable) {
		if (heightDips == FILL) {
			return FILL;
		} else if (heightDips == WRAP) {
			S32 widthNeeded = 0;
			S32 heightNeeded = 0;
			S32 vertFillCount = 0;
			S32 horizFillCount = 0;
			S32 centerNeeded = 0;
			// scale at 1.0 for dips
			S32 widthAssumption = widthAvailable;
			if (widthDips != FILL && widthDips != WRAP) {
				widthAssumption = widthDips;
			}
			calcSpaceRequired(context, 1.0f, 1.0f/context->gConfig->uiScale, &widthNeeded, &heightNeeded, &horizFillCount, &vertFillCount, &centerNeeded, widthAssumption, heightAvailable);
			return heightNeeded + paddingTopDips + paddingBottomDips;
		} else {
			return heightDips;
		}
	}

	LinearLayout::~LinearLayout() {
	}

	void LinearLayout::layout(Context *context, F32 scale) {
		//logmsg("LinearLayout::layout starting");
		//char buf[70];
		//sprintf(buf, "Laying out %d UI Components at scale %g", components->getSize(), scale);
		//logmsg(buf);
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
		S32 compLeft = 0;
		S32 compRight = 0;
		S32 compTop = 0;
		S32 compBottom = 0;
		S32 i = 0;
		calcSpaceRequired(context, scale, 1.0f, &totalWidthNeeded, &totalHeightNeeded, &horizFillCount, &vertFillCount, &centerNeeded, horizRemain, vertRemain);
		//logmsg("calcing space dist 2");
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
		//logmsg("doing components");
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
			if (component->getDesiredWidth(context, horizRemain, vertRemain) == FILL) {
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
			} else {
				// width wrap
				F32 compWidth = component->getDesiredWidth(context, horizRemain, vertRemain);
				if (compWidth == WRAP) {
					if (component->text) {
						// already in scaled units
						compWidth = context->menuRenderer->measureTextWidth(component->text);
					} else {
						compWidth = 0;
					}
					compWidth += (S32)(component->paddingLeftDips + component->paddingRightDips * scale);
				} else {
					compWidth *= scale;
				}
				// fixed-width component
				if (layoutDirection == VERTICAL) {
					if (horizAlign == LayoutParameters::LEFT) {
						compLeft = curLeft + (S32)(component->marginLeftDips * scale);
						compRight = compLeft + (S32)(compWidth);
					} if (horizAlign == LayoutParameters::HORIZONTAL_CENTER) {
						// use the difference of the left and right margins
						compLeft = left + (right - left) / 2 - (S32)(compWidth) / 2 + (S32)(component->marginLeftDips * scale - component->marginRightDips * scale);
						compRight = compLeft + (S32)(compWidth);
					} else if (horizAlign == LayoutParameters::RIGHT) {
						compRight = right - (S32)(component->marginRightDips * scale);
						compLeft = compRight - (S32)(compWidth);
					}
				} else {
					// horizontal layout
					if (horizAlign == LayoutParameters::LEFT) {
						compLeft = curLeft + (S32)(component->marginLeftDips * scale);
						compRight = compLeft + (S32)(compWidth);
						curLeft = compRight + (S32)(component->marginRightDips * scale);
					} if (horizAlign == LayoutParameters::HORIZONTAL_CENTER) {
						compLeft = left + (right - left) / 2 + - centerNeeded / 2 + centerUsed + (S32)(component->marginLeftDips * scale);
						compRight = compLeft + (S32)(compWidth);
						curLeft = compRight + (S32)(component->marginRightDips * scale);
						centerUsed += (S32)(compWidth) + (S32)(component->marginLeftDips * scale) + (S32)(component->marginRightDips * scale);
					} else if (horizAlign == LayoutParameters::RIGHT) {
						compRight = curRight - (S32)(component->marginRightDips * scale);
						compLeft = compRight - (S32)(compWidth);
						curRight = compLeft - (S32)(component->marginLeftDips * scale);
					}
				}
			}
			// vertical part
			if (component->getDesiredHeight(context, horizRemain, vertRemain) == FILL) {
				// filling vertical space
				if (layoutDirection == VERTICAL) {
					compTop = curTop + (S32)(component->marginTopDips * scale);
					compBottom = curTop + vertFillPerComponentSize - (S32)(component->marginBottomDips * scale);
					curBottom += vertFillPerComponentSize;
				} else {
					compTop = curTop + (S32)(component->marginTopDips * scale);
					compBottom = curBottom - (S32)(component->marginBottomDips * scale);
				}
			} else {
				// height wrap
				F32 compHeight = component->getDesiredHeight(context, horizRemain, vertRemain);
				if (compHeight == WRAP) {
					if (component->text) {
						if (component->isTextMultiline) {
							F32 widthAssumption = component->getDesiredWidth(context, horizRemain, vertRemain);
							if (widthAssumption == WRAP || widthAssumption == FILL) {
								widthAssumption = horizRemain;
							} else {
								widthAssumption *= scale;
							}
							widthAssumption -= (S32)((component->paddingLeftDips + component->paddingRightDips) * scale);
							// already in scaled units
							compHeight = context->menuRenderer->measureMultilineHeight(component->text, widthAssumption);
						} else {
							// already in scaled units
							compHeight = context->menuRenderer->getTextHeight();
						}
					} else {
						compHeight = 0;
					}
					compHeight += (S32)((component->paddingTopDips + component->paddingBottomDips) * scale);
				} else {
					compHeight *= scale;
				}
				// fixed-height component
				if (layoutDirection == VERTICAL) {
					if (vertAlign == LayoutParameters::TOP) {
						compTop = curTop + (S32)(component->marginTopDips * scale);
						compBottom = compTop + (S32)(compHeight);
						curTop = compBottom + (S32)(component->marginBottomDips * scale);
					} else if (vertAlign == LayoutParameters::VERTICAL_CENTER) {
						compTop = top + (bottom - top) / 2 - (centerNeeded / 2) + centerUsed + (S32)(component->marginTopDips * scale);
						S32 compTotalVertSize = (S32)(compHeight + (component->marginTopDips + component->marginBottomDips) * scale);
						compBottom = compTop + (S32)(compHeight);
						centerUsed += compTotalVertSize;
					} else if (vertAlign == LayoutParameters::BOTTOM) {
						compBottom = curBottom - (S32)(component->marginBottomDips * scale);
						compTop = compBottom - (S32)(compHeight);
						curBottom = compTop - (S32)(component->marginTopDips * scale);
					}
				} else {
					// horizontal layout
					if (vertAlign == LayoutParameters::TOP) {
						compTop = curTop + (S32)(component->marginTopDips * scale);
						compBottom = compTop + (S32)(compHeight);
					} else if (vertAlign == LayoutParameters::VERTICAL_CENTER) {
						// use the difference of the top and bottom margins
						compTop = top + (bottom - top) / 2 - (S32)(compHeight) / 2 + (S32)(component->marginTopDips * scale  - component->marginBottomDips * scale);
						compBottom = compTop + (S32)(compHeight);
					} else if (vertAlign == LayoutParameters::BOTTOM) {
						compBottom = bottom - (S32)(component->marginBottomDips * scale);
						compTop = compBottom - (S32)(compHeight);
					}
				}
			}
			component->setDrawableBounds(compLeft, compTop, compRight, compBottom);
			component->layout(context, scale);
		}
	}

	void LinearLayout::calcSpaceRequired(Context *context, F32 scale, F32 textInvScale, S32 *width, S32 *height, S32 *horizFillCount, S32 *vertFillCount, S32 *center, S32 widthAvailable, S32 heightAvailable) {
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
			S32 desiredHeight = component->getDesiredHeight(context, widthAvailable, heightAvailable);
			S32 desiredWidth = component->getDesiredWidth(context, widthAvailable, heightAvailable);
			if (component->text) {
				if (context->menuRenderer->isMultiline(component->text, widthAvailable)) {
					component->isTextMultiline = TRUE;
				}
			}
			if (layoutDirection == VERTICAL) {
				if (desiredHeight == FILL) {
					++*vertFillCount;
				} else if (desiredHeight == WRAP) {
					if (component->text) {
						if (component->isTextMultiline) {
							F32 widthAssumption = component->getDesiredWidth(context, widthAvailable, heightAvailable);
							if (widthAssumption == WRAP || widthAssumption == FILL) {
								widthAssumption = widthAvailable;
							} else {
								widthAssumption *= scale;
							}
							widthAssumption -= (S32)((component->paddingLeftDips + component->paddingRightDips) * scale);
							// text height measurements are already in screen-scaled size, but we can't always use that because for the frame layout pass
							// we need reference units (unscaled) so we multiply by textInvScale to get back to reference units (assuming it's the inverse scale)
							// and then gets worse for multiline because we need a width to use, but it's assuming that's in scaled units so we have to scale that down
							// and back
							F32 textHeight = context->menuRenderer->measureMultilineHeight(component->text, widthAssumption * (1.0f/textInvScale)) * textInvScale;
							textHeight += (S32)((component->paddingTopDips + component->paddingBottomDips) * scale);
							*height += (S32)(textHeight + component->marginTopDips * scale + component->marginBottomDips * scale);
						} else {
							F32 textHeight = context->menuRenderer->getTextHeight() * textInvScale;
							textHeight += (S32)((component->paddingTopDips + component->paddingBottomDips) * scale);
							*height += (S32)(textHeight + component->marginTopDips * scale + component->marginBottomDips * scale);
						}
					}
				} else {
					*height += (S32)(desiredHeight * scale + component->marginTopDips * scale + component->marginBottomDips * scale);
					if (vertAlign == LayoutParameters::VERTICAL_CENTER) {
						*center += (S32)(desiredHeight * scale + component->marginTopDips * scale + component->marginBottomDips * scale);
					}
				}
				// just return the widest fixed-width component
				if (desiredWidth != FILL && desiredWidth != WRAP) {
					S32 componentTotalWidthPx = (S32)((desiredWidth + component->marginLeftDips + component->marginRightDips) * scale);
					if (componentTotalWidthPx > *width) {
						*width = componentTotalWidthPx;
					}
				}
			} else {
				// horizontal
				if (desiredWidth == FILL) {
					++*horizFillCount;
				} else if (desiredWidth == WRAP) {
					if (component->text) {
						F32 textWidth = context->menuRenderer->measureTextWidth(component->text);
						textWidth += (S32)(component->paddingLeftDips + component->paddingRightDips * scale);
						*width += (S32)(textWidth + component->marginLeftDips * scale + component->marginRightDips * scale);
					}
				} else {
					*width += (S32)(desiredWidth * scale + component->marginLeftDips * scale + component->marginRightDips * scale);
					if (horizAlign == LayoutParameters::HORIZONTAL_CENTER) {
						*center += (S32)(desiredWidth * scale + component->marginLeftDips * scale + component->marginRightDips * scale);
					}
				}
				// just return the tallest fixed-height component
				if (desiredHeight != FILL && desiredHeight != WRAP) {
					S32 componentTotalHeightPx = (S32)((desiredHeight + component->marginTopDips + component->marginBottomDips) * scale);
					if (componentTotalHeightPx > *height) {
						*height = componentTotalHeightPx;
					}
				}
			}
		}
	}

	void LinearLayout::update(F32 delta) {
		Layout::update(delta);
	}

}
