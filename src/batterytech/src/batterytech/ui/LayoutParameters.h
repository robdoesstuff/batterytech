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
// Name        : LayoutParameters.h
// Description : Basic layout parameters.
// Usage       : Use with LinearLayout
//============================================================================

#ifndef LINEARLAYOUTPARAMETERS_H_
#define LINEARLAYOUTPARAMETERS_H_

#include "LayoutParameters.h"

namespace BatteryTech {

	/** \brief Standard Layout Parameters
	 * \ingroup UI
	 * \class LayoutParameters
	 *
	 */
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

}
#endif /* LINEARLAYOUTPARAMETERS_H_ */
