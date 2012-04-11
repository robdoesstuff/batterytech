/*
 * TopMenu.h
 *
 *  Created on: Dec 30, 2010
 *      Author: rgreen
 */

#ifndef TOPMENU_H_
#define TOPMENU_H_

#include "../GameContext.h"
#include <batterytech/ui/Menu.h>
#include <batterytech/ui/LinearLayout.h>
#include <batterytech/ui/Label.h>
#include <batterytech/ui/Button.h>
#include <batterytech/ui/Checkbox.h>
#include <batterytech/util/ManagedArray.h>

#define TOP_MENU_NAME "TopMenu"

using namespace BatteryTech;

class TopMenu : public Menu {
public:
	TopMenu(GameContext *context);
	virtual ~TopMenu();
	virtual void onClickUp(UIComponent *component);
	virtual void onPreShow();
	virtual void onSpecialKey(SpecialKey sKey);
private:
	GameContext *context;
	Label *titleLabel;
	Button *playButton;
	Button *settingsButton;
	Button *soundcheckButton;
	LinearLayout *mainLayout;
};

#endif /* TOPMENU_H_ */
