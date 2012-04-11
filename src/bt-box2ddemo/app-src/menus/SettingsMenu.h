/*
 * HighScoresMenu.h
 *
 *  Created on: Dec 30, 2010
 *      Author: rgreen
 */

#ifndef SETTINGSMENU_H_
#define SETTINGSMENU_H_

#include "../GameContext.h"
#include <batterytech/ui/Menu.h>
#include <batterytech/ui/LinearLayout.h>
#include <batterytech/ui/Label.h>
#include <batterytech/ui/Button.h>
#include <batterytech/ui/Checkbox.h>
#include <batterytech/util/ManagedArray.h>

#define SETTINGS_MENU_NAME "SettingsMenu"

using namespace BatteryTech;

class SettingsMenu : public Menu {
public:
	SettingsMenu(GameContext *context);
	virtual ~SettingsMenu();
	virtual void onClickUp(UIComponent *component);
	virtual void onPreShow();
	virtual void setData(void *data);
	virtual void onSpecialKey(SpecialKey sKey);
private:
	GameContext *context;
	Label *titleLabel;
	Button *backButton;
	Checkbox *soundEnabledCheckbox;
	Checkbox *vibesEnabledCheckbox;
	Checkbox *showFPSCheckbox;
	LinearLayout *mainLayout;
	const char *showMenuAfterQuitName;
};

#endif /* HIGHSCORESMENU_H_ */
