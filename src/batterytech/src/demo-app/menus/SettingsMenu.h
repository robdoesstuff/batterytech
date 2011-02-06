/*
 * HighScoresMenu.h
 *
 *  Created on: Dec 30, 2010
 *      Author: rgreen
 */

#ifndef SETTINGSMENU_H_
#define SETTINGSMENU_H_

#include <batterytech/Context.h>
#include <batterytech/ui/Menu.h>
#include <batterytech/ui/LinearLayout.h>
#include <batterytech/ui/Label.h>
#include <batterytech/ui/Button.h>
#include <batterytech/ui/Checkbox.h>
#include <batterytech/util/ManagedArray.h>

#define SETTINGS_MENU_NAME "SettingsMenu"

class SettingsMenu : public Menu {
public:
	SettingsMenu(Context *context);
	virtual ~SettingsMenu();
	virtual void onClickUp(UIComponent *component);
	virtual void onPreShow();
private:
	Context *context;
	Label *titleLabel;
	Button *backButton;
	Checkbox *soundEnabledCheckbox;
	Checkbox *vibesEnabledCheckbox;
	Checkbox *showFPSCheckbox;
	LinearLayout *mainLayout;
};

#endif /* HIGHSCORESMENU_H_ */
