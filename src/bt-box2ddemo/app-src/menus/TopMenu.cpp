/*
 * HighScoresMenu.cpp
 *
 *  Created on: Dec 30, 2010
 *      Author: rgreen
 */

#include "TopMenu.h"
#include "../UIConstants.h"
#include "../GameConstants.h"
#include <batterytech/audio/AudioManager.h>
#include <batterytech/VibrationManager.h>
#include <batterytech/util/PropertiesIO.h>
#include <batterytech/platform/platformgeneral.h>
#include <string.h>
#include "../level/LevelIO.h"
#include "MenuButtonMenu.h"
#include "SettingsMenu.h"
#include "SoundcheckMenu.h"

TopMenu::TopMenu(GameContext *context) : Menu(TOP_MENU_NAME) {
	this->context = context;
	S32 panelBgId = context->menuRenderer->addTextureAsset(UI_PANEL_2);
	S32 buttonBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1);
	S32 buttonPressedBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1_PRESSED);
	mainLayout = new LinearLayout(LinearLayout::VERTICAL);
	titleLabel = new Label("BatteryTech Demo");
	titleLabel->setHeight(60);
	titleLabel->setWidth(WRAP);
	titleLabel->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	playButton = new Button("Play");
	playButton->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	playButton->setSize(FILL, 60);
	playButton->setMargins(5);
	playButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	playButton->setBackgroundMenuResource(buttonBgId);
	playButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	settingsButton = new Button("Settings");
	settingsButton->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	settingsButton->setSize(FILL, 60);
	settingsButton->setMargins(5);
	settingsButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	settingsButton->setBackgroundMenuResource(buttonBgId);
	settingsButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	soundcheckButton = new Button("Sound Check");
	soundcheckButton->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	soundcheckButton->setSize(FILL, 60);
	soundcheckButton->setMargins(5);
	soundcheckButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	soundcheckButton->setBackgroundMenuResource(buttonBgId);
	soundcheckButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	mainLayout->setHeight(WRAP);
	mainLayout->setWidth(300);
	mainLayout->setBackgroundMenuResource(panelBgId);
	mainLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::VERTICAL_CENTER));
	mainLayout->addComponent(titleLabel);
	mainLayout->addComponent(playButton);
	mainLayout->addComponent(settingsButton);
	mainLayout->addComponent(soundcheckButton);
	setRootComponent(mainLayout);
}

TopMenu::~TopMenu() {
	delete mainLayout;
	mainLayout = NULL;
	delete titleLabel;
	titleLabel = NULL;
	delete playButton;
	playButton = NULL;
	delete settingsButton;
	settingsButton = NULL;
	delete soundcheckButton;
	soundcheckButton = NULL;
}

void TopMenu::onClickUp(UIComponent *component) {
	if (component == playButton) {
		context->uiManager->popMenu();
		context->uiManager->showMenu(MENU_BUTTON_MENU_NAME);
	} else if (component == settingsButton) {
		context->uiManager->popMenu();
		context->uiManager->showMenu(SETTINGS_MENU_NAME, (void*)TOP_MENU_NAME);
	} else if (component == soundcheckButton) {
		context->uiManager->popMenu();
		context->uiManager->showMenu(SOUNDCHECK_MENU_NAME);
	}
}

void TopMenu::onPreShow() {
}

void TopMenu::onSpecialKey(SpecialKey sKey) {
	if (sKey == SKEY_BACK) {
		_platform_exit();
	}
}
