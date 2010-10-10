/*
 * UIManager.cpp
 *
 *  Created on: Oct 8, 2010
 *      Author: rgreen
 */

#include "UIManager.h"
#include "LayoutParameters.h"

UIManager::UIManager(GraphicsConfiguration *gConfig) {
	this->gConfig = gConfig;
	menus = new ManagedArray<Menu>(10);
	activeMenuStack = new ManagedArray<Menu>(10);
}

S32 UIManager::addMenu(Menu *menu) {
	return menus->add(menu);
}

void UIManager::showMenu(S32 menuId) {
	// get menu by id, layout, push on stack
	if (menuId <= menus->lastItemIndex) {
		Menu *menu = menus->array[menuId];
		// do frame layout
		S32 width = menu->getRootComponent()->getDesiredWidth();
		S32 height = menu->getRootComponent()->getDesiredHeight();
		if (width == FILL) {
			width = gConfig->viewportWidth;
		} else {
			width = (S32) (width * gConfig->uiScale);
		}
		if (height == FILL) {
			height = gConfig->viewportHeight;
		} else {
			height = (S32) (height * gConfig->uiScale);
		}
		LayoutParameters::HorizontalAlignment horizAlign = LayoutParameters::LEFT;
		LayoutParameters::VerticalAlignment vertAlign = LayoutParameters::TOP;
		// check layout parameters
		if (menu->getRootComponent()->getLayoutParameters()) {
			horizAlign = menu->getRootComponent()->getLayoutParameters()->getHorizontalAlignment();
			vertAlign = menu->getRootComponent()->getLayoutParameters()->getVerticalAlignment();
		}
		S32 top, right, bottom, left;
		if (vertAlign == LayoutParameters::TOP) {
			top = 0;
		} else if (vertAlign == LayoutParameters::VERTICAL_CENTER) {
			top = gConfig->viewportHeight / 2 - height / 2;
		} else if (vertAlign == LayoutParameters::BOTTOM) {
			top = gConfig->viewportHeight - height;
		}
		bottom = top + height;
		if (horizAlign == LayoutParameters::LEFT) {
			left = 0;
		} else if (horizAlign == LayoutParameters::HORIZONTAL_CENTER) {
			left = gConfig->viewportWidth / 2 - width / 2;
		} else if (horizAlign == LayoutParameters::RIGHT) {
			left = gConfig->viewportWidth - width;
		}
		right = left + width;
		menu->getRootComponent()->setDrawableBounds(left, top, right, bottom);
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
