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
// Name        : Menu.h
// Description : Abstract class for creating menus.  A menu is a collection of UIComponents, Layouts and programmed behavior.
// Usage       : Subclass to make a menu and add an instance of the subclass via UIManager::addMenu()
//============================================================================

#ifndef MENU_H_
#define MENU_H_

#include "UIComponent.h"
#include <batterytech/render/MenuRenderer.h>
#include <batterytech/ui/UIManager.h>

class Menu {
public:
	Menu(const char *name) {
		this->name = name;
		isFocused = FALSE;
		layoutRequested = FALSE;
	}
	Menu(const char *name, UIComponent *component) {
		this->name = name;
		this->rootComponent = component;
		isFocused = FALSE;
		layoutRequested = FALSE;
	}
	virtual void setRootComponent(UIComponent *component) {
		this->rootComponent = component;
	}
	virtual UIComponent* getRootComponent() {
		return rootComponent;
	}
	virtual ~Menu();
	virtual void onClickDown(UIComponent *component){};
	virtual void onClickUp(UIComponent *component){};
	// called if any data was passed from the invoking showMenu call
	virtual void setData(void *data){};
	// called right before the menu is shown
	virtual void onPreShow(){};
	virtual void requestLayout() { layoutRequested = true; };
	S32 menuId;
	const char *name;
	BOOL32 isFocused;
	BOOL32 layoutRequested;
private:
	UIComponent *rootComponent;
};

#endif /* MENU_H_ */
