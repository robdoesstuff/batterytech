/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : UIManager.h
// Description : Manages the menu stack and updates UI components.
// Usage       : Add all menus using this manager and call the display, pop and clear functions as needed
//============================================================================

#ifndef UIMANAGER_H_
#define UIMANAGER_H_

#include "../primitives.h"
#include "Menu.h"
#include "../util/ManagedArray.h"
#include "../render/GraphicsConfiguration.h"
#include "../Context.h"

// cross-references
class Context;
class Menu;

class UIManager {
public:
	UIManager(Context *context);
	S32 addMenu(Menu *menu);
	void showMenu(S32 menuId);
	void showMenu(S32 menuId, void *data);
	void showMenu(const char *name);
	void showMenu(const char *name, void *data);
	S32 findMenu(const char *name);
	void popMenu();
	void clearMenuStack();
	void update();
	virtual ~UIManager();
	ManagedArray<Menu> *activeMenuStack;
	UIComponent *selectedComponent;
private:
	// true if component consumed click
	BOOL32 traverseClickState(Menu *menu, UIComponent *component, BOOL32 down, S32 x, S32 y);
	void traverseUpdate(UIComponent *component);
	void layoutMenu(Menu *menu);
	ManagedArray<Menu> *menus;
	Context *context;
	BOOL32 clickDownChecked;
	BOOL32 clickDownActive;
	S32 queuedShowMenuId;
	void *queuedData;
};

#endif /* UIMANAGER_H_ */
