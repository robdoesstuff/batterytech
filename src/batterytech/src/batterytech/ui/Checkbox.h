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
// Name        : Checkbox.h
// Description : A checkbox which can be checked or not checked.
//             : This is a compound component which consists of a button for the checkbox and a label for text.
//             : The text set for this component will show up on the right of the checkbox.
//============================================================================

#ifndef CHECKBOX_H_
#define CHECKBOX_H_

#include "LinearLayout.h"
#include "../primitives.h"
#include "Button.h"
#include "Label.h"

namespace BatteryTech {

	/** \brief A Checkbox
	 * \ingroup UI
	 * \class Checkbox
	 */
	class Checkbox: public LinearLayout {
	public:
		Checkbox(const char *text = NULL);
		void setBoxResourceId(S32 resourceId);
		void setCheckedResourceId(S32 resourceId);
		virtual void onClickDown(S32 x, S32 y);
		virtual void onClickUp(S32 x, S32 y);
		virtual void setText(const char *text);
		virtual void update(F32 delta);
		virtual ~Checkbox();
		virtual void setChecked(BOOL32 checked);
		virtual BOOL32 isChecked();
		virtual void setTextAlignment(HorizontalAlignment horizAlignment, VerticalAlignment vertAlignment);
		virtual void setTextColor(F32 r, F32 g, F32 b, F32 a);
	private:
		Button *button;
		Label *label;
		S32 boxResourceId;
		S32 checkedResourceId;
		BOOL32 checked;
	};

}
#endif /* CHECKBOX_H_ */
