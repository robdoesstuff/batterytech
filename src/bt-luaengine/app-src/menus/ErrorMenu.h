/*
 * HighScoresMenu.h
 *
 *  Created on: Dec 30, 2010
 *      Author: rgreen
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
