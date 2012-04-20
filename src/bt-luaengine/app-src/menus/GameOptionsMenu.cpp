/*
 * MainMenu.cpp
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#include "GameOptionsMenu.h"
#include <batterytech/primitives.h>
#include <batterytech/ui/SlideAnimator.h>
#include "../UIConstants.h"
#include "SettingsMenu.h"
#include "../GameContext.h"
#include "../World.h"
#include "TopMenu.h"
#include "../Game.h"

GameOptionsMenu::GameOptionsMenu(GameContext *context) : Menu(GAME_OPTIONS_MENU_NAME) {
	this->context = context;
	S32 buttonBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1);
	S32 buttonPressedBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1_PRESSED);
	//S32 panelBgId = context->menuRenderer->addTextureAsset(UI_PANEL_2);
	buttonLayout = new LinearLayout(LinearLayout::HORIZONTAL);
	settingsButton = new Button("Settings");
	settingsButton->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::BOTTOM));
	settingsButton->setSize(FILL, 60);
	settingsButton->setMargins(5);
	settingsButton->setBackgroundMenuResource(buttonBgId);
	settingsButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	settingsButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	quitButton = new Button("Quit");
	quitButton->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::BOTTOM));
	quitButton->setSize(FILL, 60);
	quitButton->setMargins(5);
	quitButton->setBackgroundMenuResource(buttonBgId);
	quitButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	quitButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	wfButton = new Button("Edit");
	wfButton->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::BOTTOM));
	wfButton->setSize(FILL, 60);
	wfButton->setMargins(5);
	wfButton->setBackgroundMenuResource(buttonBgId);
	wfButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	wfButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	resetButton = new Button("Reset");
	resetButton->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::BOTTOM));
	resetButton->setSize(FILL, 60);
	resetButton->setMargins(5);
	resetButton->setBackgroundMenuResource(buttonBgId);
	resetButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	resetButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	resetToLevelButton = new Button("Reset2Lvl");
	resetToLevelButton->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::BOTTOM));
	resetToLevelButton->setSize(FILL, 60);
	resetToLevelButton->setMargins(5);
	resetToLevelButton->setBackgroundMenuResource(buttonBgId);
	resetToLevelButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	resetToLevelButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	cancelButton = new Button("Cancel");
	cancelButton->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::BOTTOM));
	cancelButton->setSize(FILL, 60);
	cancelButton->setMargins(5);
	cancelButton->setBackgroundMenuResource(buttonBgId);
	cancelButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	cancelButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	buttonLayout->addComponent(settingsButton);
	buttonLayout->addComponent(quitButton);
	buttonLayout->addComponent(wfButton);
	buttonLayout->addComponent(resetButton);
	buttonLayout->addComponent(resetToLevelButton);
	buttonLayout->addComponent(cancelButton);
	buttonLayout->setHeight(WRAP);
	buttonLayout->setWidth(FILL);
	//buttonLayout->setBackgroundMenuResource(panelBgId);
	buttonLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::BOTTOM));
	buttonLayout->setEnterAnimator(new SlideAnimator(SlideAnimator::BOTTOM, SlideAnimator::SLIDE_IN, 0.2f));
	buttonLayout->setExitAnimator(new SlideAnimator(SlideAnimator::BOTTOM, SlideAnimator::SLIDE_OUT, 0.2f));
	setRootComponent(buttonLayout);
}

void GameOptionsMenu::onClickDown(UIComponent *component){
}

void GameOptionsMenu::onClickUp(UIComponent *component){
	this->context->uiManager->popMenu();
	if (component == quitButton) {
		this->context->uiManager->clearMenuStack();
		this->context->world->gameState = GAMESTATE_READY;
		//this->context->uiManager->showMenu(TOP_MENU_NAME);
		//this->context->uiManager->showMenu(LEVELS_MENU_NAME);
	} else if (component == settingsButton) {
		this->context->uiManager->showMenu(SETTINGS_MENU_NAME);
	} else if (component == wfButton) {
		// this->context->uiManager->showMenu(EDITOR_SIDEBAR_MENU_NAME);
		//this->context->world->wfMode = !this->context->world->wfMode;
	} else if (component == resetButton) {
		this->context->game->reset();
	} else if (component == resetToLevelButton) {
		S32 levelNumber = context->world->levelNumber;
		this->context->game->reset();
		context->uiManager->popMenu();
		context->world->levelNumber = levelNumber;
		context->world->gameState = GAMESTATE_LOADING;
	}

}

GameOptionsMenu::~GameOptionsMenu() {
	this->context = NULL;
	delete quitButton;
	delete cancelButton;
	delete wfButton;
	delete buttonLayout;
	delete resetButton;
	delete resetToLevelButton;
}

void GameOptionsMenu::onSpecialKey(SpecialKey sKey) {
	if (sKey == SKEY_BACK || sKey == SKEY_MENU) {
		this->context->uiManager->popMenu();
	}
}
