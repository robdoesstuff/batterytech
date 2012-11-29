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
// Name        : Button.h
// Description : A clickable button.
//============================================================================

#ifndef BUTTON_H_
#define BUTTON_H_

#include "UIComponent.h"

namespace BatteryTech {

	/** \brief A Button
	 * \ingroup UI
	 * \class Button
	 */
	class Button: public UIComponent {
	public:
		Button(const char *text = NULL);
		virtual ~Button();
		virtual void update(F32 delta);
	};

}
#endif /* BUTTON_H_ */
