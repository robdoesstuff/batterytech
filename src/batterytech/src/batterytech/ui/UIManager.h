/*
 * UIManager.h
 *
 *  Created on: Oct 8, 2010
 *      Author: rgreen
 */

#ifndef UIMANAGER_H_
#define UIMANAGER_H_

#include "../primitives.h"
#include "Menu.h"
#include "../util/ManagedArray.h"
#include "../render/GraphicsConfiguration.h"

class UIManager {
public:
	UIManager(GraphicsConfiguration *gConfig);
	S32 addMenu(Menu *menu);
	void showMenu(S32 menuId);
	void popMenu();
	void updateMenus(F32 delta);
	virtual ~UIManager();
private:
	ManagedArray<Menu> *menus;
	GraphicsConfiguration *gConfig;
	ManagedArray<Menu> *activeMenuStack;
};

#endif /* UIMANAGER_H_ */
