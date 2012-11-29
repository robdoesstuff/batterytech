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
// Name        : UIManager.h
// Description : Manages the menu stack and updates UI components.
// Usage       : Add all menus using this manager and call the display, pop and clear functions as needed
//============================================================================

#ifndef UIMANAGER_H_
#define UIMANAGER_H_

#include "../primitives.h"
#include "Menu.h"
#include "../util/ManagedArray.h"
#include "../render/GraphicsConfiguration.h"
#include "../Context.h"

namespace BatteryTech {

	class Context;
	class Menu;

	/** \brief A class which manages a set of Menus
	 *
	 * UIManager is responsible for managing the menu stack, dispatching input and updating the active UIComponents
	 *
	 * \ingroup UI
	 * \class UIManager
	 */
	class UIManager {
	public:
		UIManager(Context *context);
		virtual ~UIManager();
		/**
		 * Adds a menu.
		 * Call this with an instance of every menu you will be displaying in your app.
		 * This must be called before a menu can be shown.  Do not add the same menu more than once.
		 * Returns the ID of the menu instance where added.
		 */
		S32 addMenu(Menu *menu);
		/**
		 * Shows a menu by ID (inconvenient - used internally)
		 * See showMenu(const char *name)
		 */
		void showMenu(S32 menuId);
		/**
		 * Shows a menu by ID and passes data to the menu (inconvenient - used internally)
		 * See showMenu(const char *name, void *data)
		 */
		void showMenu(S32 menuId, void *data);
		/**
		 * Shows a menu using the menu's name when registered.  Preferred method.
		 * This method pushes the new menu onto the menu stack.
		 * If the last menu on the stack has an exit animator, it will be run.
		 * If this menu has an enter animator, it will be run after the other menu's exit animator has completed.
		 */
		void showMenu(const char *name);
		/**
		 * Shows a menu using the menu's name when registered.  Preferred method.
		 * This method pushes the new menu onto the menu stack.
		 * If the last menu on the stack has an exit animator, it will be run.
		 * If this menu has an enter animator, it will be run after the other menu's exit animator has completed.
		 * This method also accepts a data pointer.  Data passed will be passed to the menu via setData(void*)
		 */
		void showMenu(const char *name, void *data);
		/**
		 * Finds a menu's ID as added.  Used internally.
		 */
		S32 findMenu(const char *name);
		/**
		 * Pops the top menu off the stack.  Call this when you don't need to go immediately back to it after your menu,
		 * or if you are going to explicitly re-show that menu when you're done.
		 */
		void popMenu();
		/**
		 * Clears the entire menu stack.  No menus will be presently displayed after this is called.
		 */
		void clearMenuStack();
		/**
		 * Engine call - used to dispatch events and animate.
		 */
		void update();
		/**
		 * Direct access to the menu stack.  Use with care.
		 */
		ManagedArray<Menu> *activeMenuStack;
		/**
		 * The currently selected component.
		 */
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

}

#endif /* UIMANAGER_H_ */
