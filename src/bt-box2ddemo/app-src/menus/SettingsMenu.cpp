/*
 * HighScoresMenu.cpp
 *
 *  Created on: Dec 30, 2010
 *      Author: rgreen
 */

#include "SettingsMenu.h"
#include "../UIConstants.h"
#include "../GameConstants.h"
#include <batterytech/audio/AudioManager.h>
#include <batterytech/VibrationManager.h>
#include <batterytech/util/PropertiesIO.h>
#include <batterytech/platform/platformgeneral.h>
#include <string.h>
#include "../level/LevelIO.h"

SettingsMenu::SettingsMenu(GameContext *context) : Menu(SETTINGS_MENU_NAME) {
	this->context = context;
	S32 panelBgId = context->menuRenderer->addTextureAsset(UI_PANEL_2);
	mainLayout = new LinearLayout(LinearLayout::VERTICAL);
	S32 buttonBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1);
	S32 buttonPressedBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1_PRESSED);
	S32 boxId = context->menuRenderer->addTextureAsset(UI_CHECKBOX);
	S32 checkedBoxId = context->menuRenderer->addTextureAsset(UI_CHECKBOX_CHECKED);
	soundEnabledCheckbox = new Checkbox("Sound Enabled");
	soundEnabledCheckbox->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::TOP));
	soundEnabledCheckbox->setTextAlignment(UIComponent::LEFT, UIComponent::TOP);
	soundEnabledCheckbox->setSize(FILL, 60);
	soundEnabledCheckbox->setMargins(5);
	soundEnabledCheckbox->setBoxResourceId(boxId);
	soundEnabledCheckbox->setCheckedResourceId(checkedBoxId);
	soundEnabledCheckbox->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	vibesEnabledCheckbox = new Checkbox("Vibration Enabled");
	vibesEnabledCheckbox->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::TOP));
	vibesEnabledCheckbox->setTextAlignment(UIComponent::LEFT, UIComponent::TOP);
	vibesEnabledCheckbox->setSize(FILL, 60);
	vibesEnabledCheckbox->setMargins(5);
	vibesEnabledCheckbox->setBoxResourceId(boxId);
	vibesEnabledCheckbox->setCheckedResourceId(checkedBoxId);
	vibesEnabledCheckbox->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	showFPSCheckbox = new Checkbox("Show FPS Counter");
	showFPSCheckbox->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::TOP));
	showFPSCheckbox->setTextAlignment(UIComponent::LEFT, UIComponent::TOP);
	showFPSCheckbox->setSize(FILL, 60);
	showFPSCheckbox->setMargins(5);
	showFPSCheckbox->setBoxResourceId(boxId);
	showFPSCheckbox->setCheckedResourceId(checkedBoxId);
	showFPSCheckbox->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	backButton = new Button("Back");
	backButton->userId = -2;
	backButton->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	backButton->setSize(FILL, 60);
	backButton->setMargins(5);
	backButton->setBackgroundMenuResource(buttonBgId);
	backButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	backButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	titleLabel = new Label("Settings");
	titleLabel->setHeight(60);
	titleLabel->setWidth(100);
	titleLabel->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	mainLayout->setHeight(WRAP);
	mainLayout->setWidth(300);
	mainLayout->setBackgroundMenuResource(panelBgId);
	mainLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::VERTICAL_CENTER));
	mainLayout->addComponent(titleLabel);
	mainLayout->addComponent(soundEnabledCheckbox);
	mainLayout->addComponent(vibesEnabledCheckbox);
	mainLayout->addComponent(showFPSCheckbox);
	mainLayout->addComponent(backButton);
	setRootComponent(mainLayout);
	showMenuAfterQuitName = NULL;
}

SettingsMenu::~SettingsMenu() {
	delete mainLayout;
	mainLayout = NULL;
	delete titleLabel;
	titleLabel = NULL;
	delete backButton;
	backButton = NULL;
	delete soundEnabledCheckbox;
	soundEnabledCheckbox = NULL;
	delete vibesEnabledCheckbox;
	vibesEnabledCheckbox = NULL;
	delete showFPSCheckbox;
	showFPSCheckbox = NULL;
}

void SettingsMenu::onClickUp(UIComponent *component) {
	if (component == backButton) {
		context->uiManager->popMenu();
		if (showMenuAfterQuitName) {
			context->uiManager->showMenu(showMenuAfterQuitName);
			showMenuAfterQuitName = NULL;
		}
	} else if (component == soundEnabledCheckbox) {
		context->audioManager->setEnabled(((Checkbox*)component)->isChecked());
	} else if (component == vibesEnabledCheckbox) {
		context->vibrationManager->setEnabled(((Checkbox*)component)->isChecked());
	} else if (component == showFPSCheckbox) {
		context->showFPS = ((Checkbox*)component)->isChecked();
	}
	// save
	ManagedArray<Property> *props = new ManagedArray<Property>(3);
	if (context->audioManager->isEnabled()) {
		props->add(new Property(PREF_SOUND_ENABLED, "1"));
	} else {
		props->add(new Property(PREF_SOUND_ENABLED, "0"));
	}
	if (context->vibrationManager->isEnabled()) {
		props->add(new Property(PREF_VIBES_ENABLED, "1"));
	} else {
		props->add(new Property(PREF_VIBES_ENABLED, "0"));
	}
	if (context->showFPS) {
		props->add(new Property(PREF_SHOW_FPS, "1"));
	} else {
		props->add(new Property(PREF_SHOW_FPS, "0"));
	}
	PropertiesIO propsIO;
	LevelIO::checkDataDir();
	char path[255];
	LevelIO::getDataDirPath(path);
	strcat(path, _platform_get_path_separator());
	strcat(path, "settings.txt");
	if (!_platform_path_exists(path)) {
		char basepath[255];
		_platform_get_basename(path, basepath);
		_platform_path_create_recursive(basepath);
	}
	propsIO.saveProperties(props, path);
	props->deleteElements();
	delete props;
}

void SettingsMenu::onPreShow() {
	soundEnabledCheckbox->setChecked(context->audioManager->isEnabled());
	vibesEnabledCheckbox->setChecked(context->vibrationManager->isEnabled());
	showFPSCheckbox->setChecked(context->showFPS);
}

void SettingsMenu::setData(void *data) {
	if (data) {
		showMenuAfterQuitName = (const char*) data;
	}
}

void SettingsMenu::onSpecialKey(SpecialKey sKey) {
	if (sKey == SKEY_BACK) {
		context->uiManager->popMenu();
		if (showMenuAfterQuitName) {
			context->uiManager->showMenu(showMenuAfterQuitName);
			showMenuAfterQuitName = NULL;
		}
	}
}
