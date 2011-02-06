/*
 * MainMenu.h
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#ifndef MENUBUTTONMENU_H_
#define MENUBUTTONMENU_H_

#include <batterytech/Context.h>
#include <batterytech/ui/Menu.h>
#include <batterytech/ui/LinearLayout.h>
#include <batterytech/ui/Button.h>

#define MENU_BUTTON_MENU_NAME "MenuButtonMenu"

class MenuButtonMenu: public Menu {
public:
	MenuButtonMenu(Context *context);
	virtual ~MenuButtonMenu();
	virtual void onClickDown(UIComponent *component);
	virtual void onClickUp(UIComponent *component);
private:
	Context *context;
	LinearLayout *buttonLayout;
	Button *button1;
};

#endif /* MENUBUTTONMENU_H_ */
