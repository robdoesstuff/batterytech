/*
 * Menu.h
 *
 *  Created on: Oct 8, 2010
 *      Author: rgreen
 */

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
