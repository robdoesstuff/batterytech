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

#ifndef SCROLLABLECONTAINER_H_
#define SCROLLABLECONTAINER_H_

#include "Layout.h"

namespace BatteryTech {

	/** \brief A container that makes any children scrollable
	 * \ingroup UI
	 * \class ScrollableContainer
	 */
	class ScrollableContainer : public Layout {
	public:
		ScrollableContainer();
		ScrollableContainer(const char *text);
		virtual ~ScrollableContainer();
		virtual void addComponent(UIComponent *component);
		virtual void layout(Context *context, F32 scale);
		virtual void onClickDown(S32 x, S32 y);
		virtual void onClickUp();
		virtual void onClickMove(S32 x, S32 y);
		F32 viewXOffset;
		F32 viewYOffset;
	protected:
		S32 lastClickX;
		S32 lastClickY;
	};

}
#endif /* SCROLLABLECONTAINER_H_ */
