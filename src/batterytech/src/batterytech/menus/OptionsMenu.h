/*
 * MainMenu.h
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#ifndef OPTIONSMENU_H_
#define OPTIONSMENU_H_

#include "../ui/Menu.h"
#include "../Context.h"

#define OPTIONS_MENU_NAME "OptionsMenu"

class OptionsMenu: public Menu {
public:
	OptionsMenu(Context *context);
	virtual ~OptionsMenu();
	virtual void onClickDown(UIComponent *component);
	virtual void onClickUp(UIComponent *component);
private:
	Context *context;
};

#endif /* OPTIONSMENU_H_ */
