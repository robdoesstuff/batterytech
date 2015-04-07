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
// Name        : Label.h
// Description : A basic text label.
//============================================================================

#ifndef LABEL_H_
#define LABEL_H_

#include "UIComponent.h"

namespace BatteryTech {

	/** \brief A Text Label
	 * \ingroup UI
	 * \class Label
	 */
	class Label: public UIComponent {
	public:
		Label(const char *text = NULL) : UIComponent(text){};
		virtual ~Label();
	};

}
#endif /* LABEL_H_ */
