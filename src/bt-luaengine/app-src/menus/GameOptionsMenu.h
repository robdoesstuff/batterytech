/*
 * MainMenu.h
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#ifndef GAMEOPTIONSMENU_H_
#define GAMEOPTIONSMENU_H_

#include <batterytech/ui/Menu.h>
#include <batterytech/ui/LinearLayout.h>
#include <batterytech/ui/Button.h>

#define GAME_OPTIONS_MENU_NAME "GameOptionsMenu"

using namespace BatteryTech;
class GameContext;

class GameOptionsMenu: public Menu {
public:
	GameOptionsMenu(GameContext *context);
	virtual ~GameOptionsMenu();
	virtual void onClickDown(UIComponent *component);
	virtual void onClickUp(UIComponent *component);
	virtual void onSpecialKey(SpecialKey sKey);
private:
	GameContext *context;
	Button *settingsButton;
	Button *quitButton;
	Button *wfButton;
	Button *cancelButton;
	Button *resetButton;
	Button *resetToLevelButton;
	LinearLayout *buttonLayout;
};

#endif /* OPTIONSMENU_H_ */
