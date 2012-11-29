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
// Name        : TextField.h
// Description : An editable text field.
//============================================================================

#ifndef TEXTFIELD_H_
#define TEXTFIELD_H_

#include "UIComponent.h"

namespace BatteryTech {

	/** \brief Editable Text Field
	 * \ingroup UI
	 * \class TextField
	 */
	class TextField: public UIComponent {
	public:
		TextField(const char *text = NULL) : UIComponent(text){ allowNumeric = TRUE; };
		virtual ~TextField();
		virtual BOOL32 onKeyPressed(U8 key);
		virtual BOOL32 onKeyDown(U8 key);
		virtual BOOL32 onKeyUp(U8 key);
		virtual void onSelected();
		void setAllowNumeric(BOOL32 allowNumeric) {this->allowNumeric = allowNumeric;};
	private:
		void addChar(U8 key);
		void deleteChar();
		BOOL32 allowNumeric;
	};

}
#endif /* LABEL_H_ */
