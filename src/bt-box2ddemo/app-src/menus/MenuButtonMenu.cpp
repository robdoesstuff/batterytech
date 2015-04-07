/*
 * MainMenu.cpp
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#include "MenuButtonMenu.h"
#include <batterytech/primitives.h>
#include <batterytech/ui/SlideAnimator.h>
#include "GameOptionsMenu.h"
#include "../UIConstants.h"
#include "TopMenu.h"

MenuButtonMenu::MenuButtonMenu(GameContext *context) : Menu(MENU_BUTTON_MENU_NAME) {
	this->context = context;
	S32 buttonBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1);
	S32 buttonPressedBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1_PRESSED);
	buttonLayout = new LinearLayout(LinearLayout::HORIZONTAL);
	button1 = new Button("Menu");
	button1->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::BOTTOM));
	button1->setSize(80, 40);
	button1->setMargins(1);
	button1->setBackgroundMenuResource(buttonBgId);
	button1->setPressedBackgroundMenuResource(buttonPressedBgId);
	button1->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	buttonLayout->addComponent(button1);
	buttonLayout->setHeight(WRAP);
	buttonLayout->setWidth(WRAP);
	//buttonLayout->setBackgroundMenuResource(panelBgId);
	buttonLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::RIGHT, LayoutParameters::BOTTOM));
	buttonLayout->setEnterAnimator(new SlideAnimator(SlideAnimator::BOTTOM, SlideAnimator::SLIDE_IN, 0.5f));
	buttonLayout->setExitAnimator(new SlideAnimator(SlideAnimator::BOTTOM, SlideAnimator::SLIDE_OUT, 0.5f));
	setRootComponent(buttonLayout);
}

MenuButtonMenu::~MenuButtonMenu() {
	delete buttonLayout;
	delete button1;
	this->context = NULL;
}

void MenuButtonMenu::onClickDown(UIComponent *component){
}

void MenuButtonMenu::onClickUp(UIComponent *component){
	//this->context->uiManager->popMenu();
	// TODO - replace fields in world with LevelEditorData and add .clear() to it
	this->context->uiManager->showMenu(GAME_OPTIONS_MENU_NAME);
}

void MenuButtonMenu::onSpecialKey(SpecialKey sKey) {
	if (sKey == SKEY_BACK) {
		this->context->uiManager->clearMenuStack();
		this->context->uiManager->showMenu(TOP_MENU_NAME);
	} else if (sKey == SKEY_MENU) {
		this->context->uiManager->showMenu(GAME_OPTIONS_MENU_NAME);
	}
}
