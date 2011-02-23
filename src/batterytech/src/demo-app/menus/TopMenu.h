/*
 * TopMenu.h
 *
 *  Created on: Dec 30, 2010
 *      Author: rgreen
 */

#ifndef TOPMENU_H_
#define TOPMENU_H_

#include <batterytech/Context.h>
#include <batterytech/ui/Menu.h>
#include <batterytech/ui/LinearLayout.h>
#include <batterytech/ui/Label.h>
#include <batterytech/ui/Button.h>
#include <batterytech/ui/Checkbox.h>
#include <batterytech/util/ManagedArray.h>

#define TOP_MENU_NAME "TopMenu"

class TopMenu : public Menu {
public:
	TopMenu(Context *context);
	virtual ~TopMenu();
	virtual void onClickUp(UIComponent *component);
	virtual void onPreShow();
private:
	Context *context;
	Label *titleLabel;
	Button *playButton;
	Button *settingsButton;
	Button *soundcheckButton;
	LinearLayout *mainLayout;
};

#endif /* TOPMENU_H_ */
