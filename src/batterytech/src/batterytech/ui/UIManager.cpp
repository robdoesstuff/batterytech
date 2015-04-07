/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : UIManager.cpp
// Description : Manages the menu stack and updates UI components.
// Usage       : Add all menus using this manager and call the display, pop and clear functions as needed
//============================================================================

#include "UIManager.h"
#include "LayoutParameters.h"
#include <string.h>
#include "../Logger.h"
#include "../audio/AudioManager.h"
#include "../VibrationManager.h"
#include "../Context.h"
#include "../batterytech_globals.h"

namespace BatteryTech {

	UIManager::UIManager(Context *context) {
		this->context = context;
		menus = new ManagedArray<Menu>(100);
		activeMenuStack = new ManagedArray<Menu>(10);
		clickDownActive = FALSE;
		clickDownChecked = FALSE;
		queuedShowMenuId = -1;
		queuedData = NULL;
		selectedComponent = NULL;
	}

	S32 UIManager::addMenu(Menu *menu) {
		return menus->add(menu);
	}

	void UIManager::showMenu(const char *name) {
		showMenu(name, NULL);
	}

	void UIManager::showMenu(const char *name, void *data) {
		S32 menuId = findMenu(name);
		if (menuId != -1) {
			showMenu(menuId, data);
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
		showMenu(menuId, NULL);
	}

	void UIManager::showMenu(S32 menuId, void *data) {
		// get menu by id, layout, push on stack
		if (menuId <= menus->lastItemIndex) {
			if (activeMenuStack->getSize() > 0) {
				Menu *topMenu = activeMenuStack->array[activeMenuStack->lastItemIndex];
				if (topMenu->getRootComponent()->isExitPending()) {
					queuedShowMenuId = menuId;
					queuedData = data;
					return;
				}
			}
			queuedShowMenuId = -1;
			queuedData = NULL;
			Menu *menu = menus->array[menuId];
			menu->setData(data);
			// call the pre-show hook before we calculate anything (it may need to change out components)
			menu->onPreShow();
			layoutMenu(menu);
			menu->getRootComponent()->enter();
			activeMenuStack->add(menu);
		}
	}

	void UIManager::layoutMenu(Menu *menu) {
		GraphicsConfiguration *gConfig = context->gConfig;
		// do frame layout
		S32 width = menu->getRootComponent()->getDesiredWidth(context, gConfig->width, gConfig->height);
		S32 height = menu->getRootComponent()->getDesiredHeight(context, gConfig->width, gConfig->height);
		if (width == FILL) {
			width = gConfig->width;
		} else {
			width = (S32) (width * gConfig->uiScale);
		}
		if (height == FILL) {
			height = gConfig->height;
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
			top = gConfig->height / 2 - height / 2;
		} else if (vertAlign == LayoutParameters::BOTTOM) {
			top = gConfig->height - height;
		}
		bottom = top + height;
		if (horizAlign == LayoutParameters::LEFT) {
			left = 0;
		} else if (horizAlign == LayoutParameters::HORIZONTAL_CENTER) {
			left = gConfig->width / 2 - width / 2;
		} else if (horizAlign == LayoutParameters::RIGHT) {
			left = gConfig->width - width;
		}
		right = left + width;
		menu->getRootComponent()->setDrawableBounds(left, top, right, bottom);
		menu->getRootComponent()->layout(context, gConfig->uiScale);
	}

	void UIManager::popMenu() {
		if (activeMenuStack->lastItemIndex > -1) {
			activeMenuStack->array[activeMenuStack->lastItemIndex]->getRootComponent()->exit();
		}
		selectedComponent = NULL;
	}

	void UIManager::clearMenuStack() {
		activeMenuStack->clear();
		selectedComponent = NULL;
	}

	void UIManager::update() {
		// top menu receives clicks
		if (activeMenuStack->lastItemIndex > -1) {
			Menu *menu = activeMenuStack->array[activeMenuStack->lastItemIndex];
			//traverse menu components
			// try to deliver click state to deepest clickable component
			Context::PointerState pointer = context->pointerState[0];
			if (pointer.isDown && !clickDownChecked) {
				if (traverseClickState(menu, menu->getRootComponent(), TRUE, pointer.x, pointer.y)) {
					clickDownActive = TRUE;
					context->isUIConsumingPointers = TRUE;
				}
				clickDownChecked = TRUE;
			} else if (pointer.isDown) {
				// moving
				traverseClickState(menu, menu->getRootComponent(), TRUE, pointer.x, pointer.y);
			}
			if (!pointer.isDown) {
				if (clickDownActive) {
					// need to unclick
					traverseClickState(menu, menu->getRootComponent(), FALSE, pointer.x, pointer.y);
					context->isUIConsumingPointers = FALSE;
				}
				clickDownChecked = FALSE;
				clickDownActive = FALSE;
			}
		}
		// update all menus
		for (S32 i = 0; i < activeMenuStack->getSize(); i++) {
			Menu *menu = activeMenuStack->array[i];
			// honor any layout requests
			if (menu->layoutRequested) {
				layoutMenu(menu);
				menu->layoutRequested = FALSE;
			}
			// check for exit animation completion and queued menu pending show
			if (menu->getRootComponent()->isExitDone()) {
				activeMenuStack->remove(activeMenuStack->lastItemIndex);
				if (queuedShowMenuId != -1) {
					showMenu(queuedShowMenuId, queuedData);
				}
				continue;
			}
			traverseUpdate(menu->getRootComponent());
		}
		if (selectedComponent) {
			if (context->keyPressed) {
				if (selectedComponent->dispatchKeyPressed(context->keyPressed)) {
					context->isUIConsumingKeys = TRUE;
				}
			} else {
				for (S32 i = 0; i < MAX_KEYSTATES; i++) {
					Context::KeyState ks = context->keyState[i];
					if (ks.isDown && ks.keyCode != 0) {
						if (selectedComponent->dispatchKeyDown(ks.keyCode)) {
							context->isUIConsumingKeys = TRUE;
						}
					}
				}
			}
		} else {
			context->isUIConsumingKeys = FALSE;
		}
		// only send special keys to the menu at the very top of the stack
		if (context->specialKeyPressed != SKEY_NULL && activeMenuStack->getSize() > 0) {
			activeMenuStack->array[activeMenuStack->lastItemIndex]->onSpecialKey(context->specialKeyPressed);
		}
	}

	void UIManager::traverseUpdate(UIComponent *component) {
		component->update(context->tickDelta);
		if (component->playClickDownSound) {
			component->playClickDownSound = FALSE;
			context->audioManager->playSound(component->clickDownSoundAsset, 0, 1.0f, 1.0f, 1.0f);
		}
		if (component->playClickUpSound) {
			component->playClickUpSound = FALSE;
			context->audioManager->playSound(component->clickUpSoundAsset, 0, 1.0f, 1.0f, 1.0f);
		}
		if (component->playClickDownVibeEffect) {
			component->playClickDownVibeEffect = FALSE;
			context->vibrationManager->playEffect(component->clickDownVibeEffectId, 1.0);
		}
		if (component->playClickUpVibeEffect) {
			component->playClickUpVibeEffect = FALSE;
			context->vibrationManager->playEffect(component->clickUpVibeEffectId, 1.0);
		}
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
		if (component->isEnterPending() || component->isExitPending()) {
			return FALSE;
		}
		BOOL32 isClickUnder = FALSE;
		ManagedArray<UIComponent> *subComps = component->components;
		if (subComps) {
			S32 i;
			for (i = 0; i < subComps->getSize(); i++) {
				UIComponent *subComp = subComps->array[i];
				if (traverseClickState(menu, subComp, down, x, y)) {
					if (!component->isClickableUnderChildren) {
						return TRUE;
					} else {
						isClickUnder = TRUE;
						break;
					}
				}
			}
		}
		if (clickDownActive && down) {
			// move
			if (isClickUnder || component == selectedComponent) {
				component->dispatchClickMove(x, y);
				return TRUE;
			}
		} else if (down && component->isClickable && component->isEnabled) {
			if (x < component->virtualLeft || x > component->virtualRight || y < component->virtualTop || y > component->virtualBottom) {
				// do nothing, fast out check
			} else {
				if (!isClickUnder) {
					if (component->isSelectable) {
						component->onSelected();
						selectedComponent = component;
					} else {
						selectedComponent = NULL;
					}
				}
				component->isPressed = TRUE;
				component->dispatchClickDown(x, y);
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
		logmsg("Releasing UI Manager");
		context = NULL;
		if (menus) {
			menus->deleteElements();
		}
		delete menus;
		menus = NULL;
		delete activeMenuStack;
		activeMenuStack = NULL;
		selectedComponent = NULL;
	}

}
