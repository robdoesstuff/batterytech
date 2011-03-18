/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : LinearLayout.h
// Description : A layout that places components from left to right (horizontal) or top to bottom (vertical)
// Usage       : Every component in this layout must have LayoutParameters added to it.
//             : Components will be placed depending on the size and values of the layout parameters.
//============================================================================

#ifndef LINEARLAYOUT_H_
#define LINEARLAYOUT_H_

#include "../primitives.h"
#include "Layout.h"

namespace BatteryTech {

	class Context;

	class LinearLayout: public Layout {
	public:
		enum LayoutDirection { VERTICAL, HORIZONTAL };
		LinearLayout(const char *text = NULL);
		LinearLayout(LayoutDirection direction);
		virtual ~LinearLayout();
		virtual void layout(Context *context, F32 scale);
		virtual void update(F32 delta);
		virtual S32 getDesiredWidth(Context *context, S32 widthAvailable, S32 heightAvailable);
		virtual S32 getDesiredHeight(Context *context, S32 widthAvailable, S32 heightAvailable);
	protected:
		LayoutDirection layoutDirection;
	private:
		void calcSpaceRequired(Context *context, F32 scale, S32 *width, S32 *height, S32 *horizFillCount, S32 *vertFillCount, S32 *center, S32 widthAvailable, S32 heightAvailable);
	};

}
#endif /* LINEARLAYOUT_H_ */
