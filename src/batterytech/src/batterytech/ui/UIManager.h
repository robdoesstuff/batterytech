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
