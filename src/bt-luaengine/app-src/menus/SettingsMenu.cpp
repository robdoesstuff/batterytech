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
#include <batterytech/ui/SlideAnimator.h>
#include "../Game.h"

#define SETTINGS_MENU_BG "ui/settings_menu_bg_tex.png"

SettingsMenu::SettingsMenu(GameContext *context) : Menu(SETTINGS_MENU_NAME) {
	this->context = context;
	S32 panelBgId = context->menuRenderer->addTextureAsset(SETTINGS_MENU_BG);
	mainLayout = new LinearLayout(LinearLayout::VERTICAL);
	S32 buttonBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1);
	S32 buttonPressedBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1_PRESSED);
	S32 boxId = context->menuRenderer->addTextureAsset(UI_CHECKBOX);
	S32 checkedBoxId = context->menuRenderer->addTextureAsset(UI_CHECKBOX_CHECKED);
	soundEnabledCheckbox = new Checkbox("Sound Enabled");
	soundEnabledCheckbox->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::TOP));
	soundEnabledCheckbox->setTextAlignment(UIComponent::LEFT, UIComponent::TOP);
	soundEnabledCheckbox->setSize(FILL, 50);
	soundEnabledCheckbox->setBoxResourceId(boxId);
	soundEnabledCheckbox->setCheckedResourceId(checkedBoxId);
	soundEnabledCheckbox->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	soundEnabledCheckbox->marginLeftDips = 5;
	musicEnabledCheckbox = new Checkbox("Music Enabled");
	musicEnabledCheckbox->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::TOP));
	musicEnabledCheckbox->setTextAlignment(UIComponent::LEFT, UIComponent::TOP);
	musicEnabledCheckbox->setSize(FILL, 50);
	musicEnabledCheckbox->setBoxResourceId(boxId);
	musicEnabledCheckbox->setCheckedResourceId(checkedBoxId);
	musicEnabledCheckbox->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	musicEnabledCheckbox->marginLeftDips = 5;
	vibesEnabledCheckbox = new Checkbox("Vibration Enabled");
	vibesEnabledCheckbox->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::TOP));
	vibesEnabledCheckbox->setTextAlignment(UIComponent::LEFT, UIComponent::TOP);
	vibesEnabledCheckbox->setSize(FILL, 50);
	vibesEnabledCheckbox->setBoxResourceId(boxId);
	vibesEnabledCheckbox->setCheckedResourceId(checkedBoxId);
	vibesEnabledCheckbox->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	vibesEnabledCheckbox->marginLeftDips = 5;
	showFPSCheckbox = new Checkbox("Show FPS Counter");
	showFPSCheckbox->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::TOP));
	showFPSCheckbox->setTextAlignment(UIComponent::LEFT, UIComponent::TOP);
	showFPSCheckbox->setSize(FILL, 50);
	showFPSCheckbox->setBoxResourceId(boxId);
	showFPSCheckbox->setCheckedResourceId(checkedBoxId);
	showFPSCheckbox->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	showFPSCheckbox->marginLeftDips = 5;
	ofSettingsButton = new Button("OpenFeint");
	ofSettingsButton->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	ofSettingsButton->setSize(280, 60);
	ofSettingsButton->setMargins(5);
	ofSettingsButton->setBackgroundMenuResource(buttonBgId);
	ofSettingsButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	ofSettingsButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	backButton = new Button("Back");
	backButton->userId = -2;
	backButton->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	backButton->setSize(280, 60);
	backButton->setMargins(5);
	backButton->setBackgroundMenuResource(buttonBgId);
	backButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	backButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	titleLabel = new Label("Settings");
	titleLabel->setHeight(60);
	titleLabel->setWidth(150);
	titleLabel->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	titleLabel->setTextAlignment(UIComponent::HORIZONTAL_CENTER, UIComponent::VERTICAL_CENTER);
	titleLabel->marginTopDips=5;
	mainLayout->setHeight(WRAP);
	mainLayout->setWidth(380);
	mainLayout->setBackgroundMenuResource(panelBgId);
	mainLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::VERTICAL_CENTER));
	mainLayout->addComponent(titleLabel);
	mainLayout->addComponent(soundEnabledCheckbox);
	//mainLayout->addComponent(musicEnabledCheckbox);
	mainLayout->addComponent(vibesEnabledCheckbox);
	mainLayout->addComponent(showFPSCheckbox);
	mainLayout->addComponent(ofSettingsButton);
	mainLayout->addComponent(backButton);
	mainLayout->setEnterAnimator(new SlideAnimator(SlideAnimator::TOP, SlideAnimator::SLIDE_IN, 0.3f));
	mainLayout->setExitAnimator(new SlideAnimator(SlideAnimator::BOTTOM, SlideAnimator::SLIDE_OUT, 0.3f));
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
	delete musicEnabledCheckbox;
	musicEnabledCheckbox = NULL;
	delete vibesEnabledCheckbox;
	vibesEnabledCheckbox = NULL;
	delete showFPSCheckbox;
	showFPSCheckbox = NULL;
	delete ofSettingsButton;
	ofSettingsButton = NULL;
}

void SettingsMenu::onClickUp(UIComponent *component) {
	if (component == backButton) {
		context->uiManager->popMenu();
		if (showMenuAfterQuitName) {
			context->uiManager->showMenu(showMenuAfterQuitName);
			showMenuAfterQuitName = NULL;
		}
	} else if (component == soundEnabledCheckbox) {
		BOOL32 enable = ((Checkbox*)component)->isChecked();
		context->audioManager->setEnabled(enable);
		if (!enable) {
			context->audioManager->stopAllSounds();
		}
		context->game->luaBinder->callGlobalVA("soundEnabled", "b>", enable);
	} else if (component == vibesEnabledCheckbox) {
		context->vibrationManager->setEnabled(((Checkbox*)component)->isChecked());
	} else if (component == showFPSCheckbox) {
		context->showFPS = ((Checkbox*)component)->isChecked();
	} else if (component == ofSettingsButton) {
		_platform_hook("showOFSettings", NULL, 0);
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
	char path[1024];
	if (LevelIO::createFileRecursive("settings.txt", path)) {
		propsIO.saveProperties(props, path);
	}
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
