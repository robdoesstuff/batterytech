/*
 * Menu.h
 *
 *  Created on: Oct 8, 2010
 *      Author: rgreen
 */

#ifndef MENU_H_
#define MENU_H_

#include "UIComponent.h"

class Menu {
public:
	Menu(const char *name) {
		this->name = name;
		isFocused = FALSE;
	}
	Menu(const char *name, UIComponent *component) {
		this->name = name;
		this->rootComponent = component;
		isFocused = FALSE;
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
	S32 menuId;
	const char *name;
	BOOL32 isFocused;
private:
	UIComponent *rootComponent;
};

#endif /* MENU_H_ */
