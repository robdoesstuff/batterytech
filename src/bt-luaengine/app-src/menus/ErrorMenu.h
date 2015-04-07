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

#ifndef ERRORMENU_H_
#define ERRORMENU_H_

#include <batterytech/ui/Menu.h>
#include <batterytech/ui/LinearLayout.h>
#include <batterytech/ui/Label.h>
#include <batterytech/ui/Button.h>
#include <batterytech/ui/Checkbox.h>
#include <batterytech/util/ManagedArray.h>
#include "../GameContext.h"

#define ERROR_MENU_NAME "ErrorMenu"

using namespace BatteryTech;

struct ErrorMessage {
	const char *title;
	const char *message;
	bool showRestart;
	S32 width;
	ErrorMessage(const char *title, const char *message, S32 width=0) {
		this->title = title;
		this->message = message;
		this->showRestart = true;
		this->width = width;
	}
	ErrorMessage(const char *title, const char *message, bool showRestart, S32 width=0) {
		this->title = title;
		this->message = message;
		this->showRestart = showRestart;
		this->width = width;
	}
};

class ErrorMenu : public Menu {
public:
	ErrorMenu(GameContext *context);
	virtual ~ErrorMenu();
	virtual void onClickUp(UIComponent *component);
	virtual void onPreShow();
	virtual void setData(void *data);
	virtual void onSpecialKey(SpecialKey sKey);
private:
	GameContext *context;
	Label *titleLabel;
	Label *errorMessage;
	Button *restartButton;
	Button *okButton;
	LinearLayout *mainLayout;
};

#endif /* HIGHSCORESMENU_H_ */
