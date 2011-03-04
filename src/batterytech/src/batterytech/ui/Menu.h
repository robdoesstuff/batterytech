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

namespace BatteryTech {

	class Menu {
	public:
		/**
		 * Name constructor.  Every menu must have a name.
		 * The name is used in UIManager::showMenu(name) to push the menu onto the stack and display it.
		 */
		Menu(const char *name) {
			this->name = name;
			isFocused = FALSE;
			layoutRequested = FALSE;
		}
		/**
		 * Name and root component constructor.
		 * Every menu must have a name.
		 * The name is used in UIManager::showMenu(name) to push the menu onto the stack and display it.
		 * The root component is the base component of the hierarchy that will be rendered.
		 */
		Menu(const char *name, UIComponent *component) {
			this->name = name;
			this->rootComponent = component;
			isFocused = FALSE;
			layoutRequested = FALSE;
		}
		/**
		 * Sets the root component of this menu
		 * The root component is the base component of the hierarchy that will be rendered.
		 */
		virtual void setRootComponent(UIComponent *component) {
			this->rootComponent = component;
		}
		/**
		 * Gets the root component of this menu
		 */
		virtual UIComponent* getRootComponent() {
			return rootComponent;
		}
		virtual ~Menu();
		/**
		 * Override to handle click down events for a component.
		 * The component is a component in this menu's component hierarchy, starting at the root.
		 */
		virtual void onClickDown(UIComponent *component){};
		/**
		 * Override to handle click up events for a component.
		 * The component is a component in this menu's component hierarchy, starting at the root.
		 */
		virtual void onClickUp(UIComponent *component){};
		/**
		 * Override to handle custom data passed before show
		 * called if any data was passed from the invoking UIManager::showMenu call
		 */
		virtual void setData(void *data){};
		/**
		 * Override to perform any task before the menu is shown
		 * called right before the menu is shown
		 */
		virtual void onPreShow(){};
		/**
		 * Call this to request that the menu (using root component) performs a new layout.
		 * Useful if number of components or sizes have changed
		 */
		virtual void requestLayout() { layoutRequested = true; };
		/**
		 * The menu's ID (used by UIManager)
		 */
		S32 menuId;
		/**
		 * The menu's name (used to show menus by name)
		 */
		const char *name;
		/**
		 * If the menu is currently focused
		 */
		BOOL32 isFocused;
		/**
		 * If a layout has been requested
		 */
		BOOL32 layoutRequested;
	private:
		UIComponent *rootComponent;
	};

}
#endif /* MENU_H_ */
