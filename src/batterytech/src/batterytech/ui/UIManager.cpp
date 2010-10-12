/*
 * UIManager.cpp
 *
 *  Created on: Oct 8, 2010
 *      Author: rgreen
 */

#include "UIManager.h"
#include "LayoutParameters.h"
#include <string.h>
#include "../Logger.h"

UIManager::UIManager(Context *context) {
	this->context = context;
	menus = new ManagedArray<Menu>(10);
	activeMenuStack = new ManagedArray<Menu>(10);
	clickDownActive = FALSE;
	clickDownChecked = FALSE;
	queuedShowMenuId = -1;
}

S32 UIManager::addMenu(Menu *menu) {
	return menus->add(menu);
}

void UIManager::showMenu(const char *name) {
	S32 menuId = findMenu(name);
	if (menuId != -1) {
		showMenu(menuId);
	} else {
		logmsg("No menu found:");
		logmsg(name);
	}
}

S32 UIManager::findMenu(const char *name) {
	S32 i;
	for (i = 0; i < menus->getSize(); i++) {
		if (strcmp(name, menus->array[i]->name) == 0) {
			return i;
		}
	}
	return -1;
}

void UIManager::showMenu(S32 menuId) {
	// get menu by id, layout, push on stack
	if (menuId <= menus->lastItemIndex) {
		if (activeMenuStack->getSize() > 0) {
			Menu *topMenu = activeMenuStack->array[activeMenuStack->lastItemIndex];
			if (topMenu->getRootComponent()->isExitPending()) {
				queuedShowMenuId = menuId;
				return;
			}
		}
		queuedShowMenuId = -1;
		GraphicsConfiguration *gConfig = context->gConfig;
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
		menu->getRootComponent()->enter();
		activeMenuStack->add(menu);
	}
}

void UIManager::popMenu() {
	if (activeMenuStack->lastItemIndex > -1) {
		activeMenuStack->array[activeMenuStack->lastItemIndex]->getRootComponent()->exit();
	}
}

void UIManager::update() {
	// top menu receives clicks
	if (activeMenuStack->lastItemIndex > -1) {
		Menu *menu = activeMenuStack->array[activeMenuStack->lastItemIndex];
		//traverse menu components
		// try to deliver click state to deepest clickable component
		if (context->down1 && !clickDownChecked) {
			if (traverseClickState(menu, menu->getRootComponent(), TRUE, context->x1, context->y1)) {
				clickDownActive = TRUE;
			}
			clickDownChecked = TRUE;
		}
		if (!context->down1) {
			if (clickDownActive) {
				// need to unclick
				traverseClickState(menu, menu->getRootComponent(), FALSE, context->x1, context->y1);
			}
			clickDownChecked = FALSE;
			clickDownActive = FALSE;
		}
	}
	// update all menus
	S32 i;
	for (i = 0; i < activeMenuStack->getSize(); i++) {
		Menu *menu = activeMenuStack->array[i];
		if (menu->getRootComponent()->isExitDone()) {
			activeMenuStack->remove(activeMenuStack->lastItemIndex);
			if (queuedShowMenuId != -1) {
				showMenu(queuedShowMenuId);
			}
			continue;
		}
		traverseUpdate(menu->getRootComponent());
	}
}

void UIManager::traverseUpdate(UIComponent *component) {
	component->update(context->tickDelta);
	ManagedArray<UIComponent> *subComps = component->components;
	if (subComps) {
		S32 i;
		for (i = 0; i < subComps->getSize(); i++) {
			UIComponent *subComp = subComps->array[i];
			traverseUpdate(subComp);
		}
	}
}

BOOL32 UIManager::traverseClickState(Menu *menu, UIComponent *component, BOOL32 down, S32 x, S32 y) {
	// don't be clickable until animated in (this is first to catch parents first)
	if (component->isEnterPending()) {
		return FALSE;
	}
	ManagedArray<UIComponent> *subComps = component->components;
	if (subComps) {
		S32 i;
		for (i = 0; i < subComps->getSize(); i++) {
			UIComponent *subComp = subComps->array[i];
			if (traverseClickState(menu, subComp, down, x, y)) {
				return TRUE;
			}
		}
	}
	if (down && component->isClickable) {
		if (x < component->left || x > component->right || y < component->top || y > component->bottom) {
			// do nothing, fast out check
		} else {
			component->isPressed = TRUE;
			component->dispatchClickDown();
			menu->onClickDown(component);
			return TRUE;
		}
	}
	if (!down && component->isPressed) {
		component->isPressed = FALSE;
		component->dispatchClickUp();
		menu->onClickUp(component);
		return FALSE;
	}
	return FALSE;
}

UIManager::~UIManager() {
	context = NULL;
	// TODO - also delete each menu
	delete menus;
	delete activeMenuStack;
}
