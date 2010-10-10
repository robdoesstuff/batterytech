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
#include "../Context.h"

// cross-references
class Context;

class UIManager {
public:
	UIManager(Context *context);
	S32 addMenu(Menu *menu);
	void showMenu(S32 menuId);
	void popMenu();
	void update();
	virtual ~UIManager();
	ManagedArray<Menu> *activeMenuStack;
private:
	ManagedArray<Menu> *menus;
	Context *context;
};

#endif /* UIMANAGER_H_ */
