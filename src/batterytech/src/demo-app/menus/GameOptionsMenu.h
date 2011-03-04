/*
 * MainMenu.h
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#ifndef GAMEOPTIONSMENU_H_
#define GAMEOPTIONSMENU_H_

#include <batterytech/Context.h>
#include <batterytech/ui/Menu.h>
#include <batterytech/ui/LinearLayout.h>
#include <batterytech/ui/Button.h>

#define GAME_OPTIONS_MENU_NAME "GameOptionsMenu"

using namespace BatteryTech;

class GameOptionsMenu: public Menu {
public:
	GameOptionsMenu(Context *context);
	virtual ~GameOptionsMenu();
	virtual void onClickDown(UIComponent *component);
	virtual void onClickUp(UIComponent *component);
private:
	Context *context;
	Button *settingsButton;
	Button *quitButton;
	Button *cancelButton;
	LinearLayout *buttonLayout;
};

#endif /* OPTIONSMENU_H_ */
