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
// Name        : ScrollableContainer.h
// Description : Wraps a UIComponent to allow it to have larger dimensions than screen space
// Usage       : Instantiate and place a component or layout inside to make scrollable
//============================================================================

#include "ScrollableContainer.h"
#include "../util/ManagedArray.h"
#include "../Logger.h"
#include "../Context.h"
#include "../render/MenuRenderer.h"

namespace BatteryTech {
    ScrollableContainer::ScrollableContainer() : Layout(text) {
		viewXOffset = 0;
		viewYOffset = 0;
		lastClickX = 0;
		lastClickY = 0;
		isClickableUnderChildren = TRUE;
    }
    
	ScrollableContainer::ScrollableContainer(const char *text) : Layout(text) {
		viewXOffset = 0;
		viewYOffset = 0;
		lastClickX = 0;
		lastClickY = 0;
		isClickableUnderChildren = TRUE;
	}

	ScrollableContainer::~ScrollableContainer() {
	}

	void ScrollableContainer::addComponent(UIComponent *component) {
		if (components->getSize() > 0) {
			logmsg("Error - ScrollableContainer can only have one component!");
		} else {
			Layout::addComponent(component);
		}
	}

	void ScrollableContainer::layout(Context *context, F32 scale) {
		// layout is always called after drawable bounds are set
		S32 curLeft = (S32)(left + paddingLeftDips * scale);
		S32 curRight = (S32)(right - paddingRightDips * scale);
		S32 curTop = (S32)(top + paddingTopDips * scale);
		S32 curBottom = (S32)(bottom - paddingBottomDips * scale);
		S32 widthAvailable = curRight - curLeft;
		S32 heightAvailable = curBottom - curTop;
		S32 compLeft = 0;
		S32 compRight = 0;
		S32 compTop = 0;
		S32 compBottom = 0;
		for (S32 i = 0; i < components->getSize(); i++) {
			UIComponent *component = components->array[i];
			S32 desiredHeight = component->getDesiredHeight(context, widthAvailable, heightAvailable);
			S32 desiredWidth = component->getDesiredWidth(context, widthAvailable, heightAvailable);
			if (component->text) {
				if (context->menuRenderer->isMultiline(component->text, widthAvailable)) {
					component->isTextMultiline = TRUE;
				}
			}
			// we always want to give the requested size back to the component or calc wrapped size or use our size for fill
			// Height
			if (desiredHeight == FILL) {
				compTop = curTop + component->marginTopDips * scale;
				compBottom = curBottom - component->marginTopDips * scale;
			} else {
				S32 compHeight = desiredHeight;
				if (desiredHeight == WRAP) {
					if (component->isTextMultiline) {
						F32 widthAssumption = desiredWidth;
						if (widthAssumption == WRAP || widthAssumption == FILL) {
							widthAssumption = widthAvailable;
						} else {
							widthAssumption *= scale;
						}
						widthAssumption -= (S32)(component->paddingLeftDips + component->paddingRightDips * scale);
						// already in scaled units
						compHeight = context->menuRenderer->measureMultilineHeight(component->text, widthAssumption);
					} else {
						// already in scaled units
						compHeight = context->menuRenderer->getTextHeight();
					}
				} else {
					compHeight *= scale;
				}
				compTop = curTop + component->marginTopDips * scale;
				compBottom = curTop + compHeight;
			}
			// Width
			if (desiredWidth == FILL) {
				compLeft = curLeft + component->marginLeftDips * scale;
				compRight = curRight - component->marginRightDips * scale;
			} else {
				F32 compWidth = desiredWidth;
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
				compLeft = curLeft + component->marginLeftDips * scale;
				compRight = curLeft + compWidth + component->marginRightDips * scale;
			}
			component->setDrawableBounds(left, top, right, bottom);
			component->setVirtualBounds(compLeft, compTop, compRight, compBottom);
			component->layout(context, scale);
		}
	}

	void ScrollableContainer::onClickDown(S32 x, S32 y) {
		lastClickX = x;
		lastClickY = y;
		char buf[255];
		sprintf(buf, "ClickDown %d %d", x, y);
		logmsg(buf);
	}

	void ScrollableContainer::onClickMove(S32 x, S32 y) {
		viewXOffset += x - lastClickX;
		viewYOffset += y - lastClickY;
		for (S32 i = 0; i < components->getSize(); i++) {
			UIComponent *component = components->array[i];
			component->scrollVirtualBounds(x - lastClickX, y - lastClickY);
			if (component->virtualTop > component->top) {
				component->scrollVirtualBounds(0, -(component->virtualTop - component->top));
			}
			if (component->virtualBottom < component->bottom) {
				component->scrollVirtualBounds(0, (component->bottom - component->virtualBottom));
			}
			if (component->virtualLeft > component->left) {
				component->scrollVirtualBounds(-(component->virtualLeft - component->left), 0);
			}
			if (component->virtualRight < component->right) {
				component->scrollVirtualBounds((component->right - component->virtualRight), 0);
			}
		}
		lastClickX = x;
		lastClickY = y;
		//char buf[255];
		//sprintf(buf, "ClickMove %d %d ViewOffset %f %f", x, y, viewXOffset, viewYOffset);
		//logmsg(buf);
	}

	void ScrollableContainer::onClickUp() {

	}

}
