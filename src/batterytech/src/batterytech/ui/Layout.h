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
// Name        : Layout.h
// Description : Abstract class for creating layouts of varying behavior.
// Usage       : Subclass to create a layout and add layout behavior.
//============================================================================

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "UIComponent.h"

namespace BatteryTech {

	/** \brief Base class for a layout
	 * \ingroup UI
	 * \class Layout
	 */
	class Layout: public UIComponent {
	public:
		Layout(const char *text = NULL) : UIComponent(text){};
		virtual ~Layout();
	};

}
#endif /* LAYOUT_H_ */
