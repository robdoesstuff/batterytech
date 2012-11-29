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
// Name        : TextField.cpp
// Description : An editable text field.
//============================================================================

#include "TextField.h"
#include <string.h>
#include "../platform/platformgeneral.h"

namespace BatteryTech {

	TextField::~TextField() {
	}

	BOOL32 TextField::onKeyPressed(U8 key) {
		if (key == 8) {
			deleteChar();
			return TRUE;
		} else if (key >= 32 && key < 127) {
			if (!allowNumeric && key > 47 && key < 58) {
				// ignore
			} else {
				addChar(key);
				return TRUE;
			}
		} else if (key == 13 || key == 10) {
			// enter/return/newline
			_platform_hide_keyboard();
			return TRUE;
		}
		return FALSE;
	}

	BOOL32 TextField::onKeyDown(U8 key) {
		if (key == 8 || key == 10 || key == 13 || (key >= 32 && key < 127)) {
			if (!allowNumeric && key > 47 && key < 58) {
				return FALSE;
			} else {
				return TRUE;
			}
			return TRUE;
		}
		return FALSE;
	}

	BOOL32 TextField::onKeyUp(U8 key) {
		if (key == 8 || key == 10 || key == 13 || (key >= 32 && key < 127)) {
			if (!allowNumeric && key > 47 && key < 58) {
				return FALSE;
			} else {
				return TRUE;
			}
			return TRUE;
		}
		return FALSE;
	}

	void TextField::addChar(U8 key) {
		if (text) {
			S32 textLength = strlen(text);
			char *oldText = text;
			text = new char[textLength + 2];
			strcpy(text, oldText);
			delete [] oldText;
			text[textLength] = key;
			text[textLength + 1] = '\0';
		} else {
			this->text = new char[2];
			text[0] = key;
			text[1] = '\0';
		}
	}

	void TextField::deleteChar() {
		if (text) {
			S32 textLength = strlen(text);
			if (textLength == 1) {
				delete [] text;
				text = NULL;
			} else {
				char *oldText = text;
				text = new char[textLength];
				strncpy(text, oldText, textLength - 1);
				delete [] oldText;
				text[textLength - 1] = '\0';
				logmsg(text);
			}
		}
	}

	void TextField::onSelected() {
		_platform_show_keyboard();
	}

}
