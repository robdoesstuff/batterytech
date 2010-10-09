/*
 * UIManager.cpp
 *
 *  Created on: Oct 8, 2010
 *      Author: rgreen
 */

#include "UIManager.h"

UIManager::UIManager(GraphicsConfiguration *gConfig) {
	this->gConfig = gConfig;
	menus = new ManagedArray<Menu>(10);
	activeMenuStack = new ManagedArray<Menu>(10);
}

S32 UIManager::addMenu(Menu *menu) {
	menus->add(menu);
	return menus->lastItemIndex;
}

void UIManager::showMenu(S32 menuId) {
	// get menu by id, layout, push on stack
	if (menuId <= menus->lastItemIndex) {
		Menu *menu = menus->array[menuId];
		menu->getRootComponent()->setDrawableBounds(0, 0, gConfig->viewportWidth, gConfig->viewportHeight);
		menu->getRootComponent()->layout(gConfig->uiScale);
		activeMenuStack->add(menu);
	}
}

void UIManager::popMenu() {
	if (activeMenuStack->lastItemIndex > -1) {
		activeMenuStack->remove(activeMenuStack->lastItemIndex);
	}
}

void UIManager::updateMenus(F32 delta) {
	// TODO - update every component of every active menu
}

UIManager::~UIManager() {
	gConfig = NULL;
	// TODO - also delete each menu
	delete menus;
	delete activeMenuStack;
}
