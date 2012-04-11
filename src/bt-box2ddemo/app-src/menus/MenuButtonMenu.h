/*
 * MainMenu.h
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#ifndef MENUBUTTONMENU_H_
#define MENUBUTTONMENU_H_

#include "../GameContext.h"
#include <batterytech/ui/Menu.h>
#include <batterytech/ui/LinearLayout.h>
#include <batterytech/ui/Button.h>

#define MENU_BUTTON_MENU_NAME "MenuButtonMenu"

using namespace BatteryTech;

class MenuButtonMenu: public Menu {
public:
	MenuButtonMenu(GameContext *context);
	virtual ~MenuButtonMenu();
	virtual void onClickDown(UIComponent *component);
	virtual void onClickUp(UIComponent *component);
	virtual void onSpecialKey(SpecialKey sKey);
private:
	GameContext *context;
	LinearLayout *buttonLayout;
	Button *button1;
};

#endif /* MENUBUTTONMENU_H_ */
