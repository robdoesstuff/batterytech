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

	/** \brief A dynamic layout which can be nested and is either Vertical or Horizontal
	 *
	 * In Vertical mode, this layout will stack children on top of each other.  In Horizontal mode, this layout will set children side-by-side.
	 *
	 * Children can be aligned to the top, center or bottom vertically as well as left, center and right horizontally all mutually inclusive which
	 * allows for a number of layout options, including top, center and bottom of the screen for a full screen layout with dynamic height for the
	 * center component.  This is a very powerful layout and with nesting, virtually any layout result with the exception of a dynamic row/column
	 * grid can be achieved.
	 *
	 * \ingroup UI
	 * \class LinearLayout
	 */
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
		void calcSpaceRequired(Context *context, F32 scale, F32 textInvScale, S32 *width, S32 *height, S32 *horizFillCount, S32 *vertFillCount, S32 *center, S32 widthAvailable, S32 heightAvailable);
	};

}
#endif /* LINEARLAYOUT_H_ */
