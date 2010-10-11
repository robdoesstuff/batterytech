/*
 * MainMenu.h
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "../ui/Menu.h"
#include "../Context.h"

class MainMenu: public Menu {
public:
	MainMenu(Context *context);
	virtual ~MainMenu();
	virtual void onClickDown(UIComponent *component);
	virtual void onClickUp(UIComponent *component);
private:
	Context *context;
};

#endif /* MAINMENU_H_ */
