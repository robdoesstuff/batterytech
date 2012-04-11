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
// Name        : SoundcheckMenu.h
// Description : Example menu to test audio
//============================================================================


#include "SoundcheckMenu.h"
#include "../UIConstants.h"
#include "../GameConstants.h"
#include <batterytech/audio/AudioManager.h>
#include <batterytech/VibrationManager.h>
#include <batterytech/util/PropertiesIO.h>
#include <batterytech/platform/platformgeneral.h>
#include <string.h>
#include "../level/LevelIO.h"
#include "TopMenu.h"

SoundcheckMenu::SoundcheckMenu(GameContext *context) : Menu(SOUNDCHECK_MENU_NAME) {
	this->context = context;
	S32 panelBgId = context->menuRenderer->addTextureAsset(UI_PANEL_2);
	mainLayout = new LinearLayout(LinearLayout::VERTICAL);
	S32 buttonBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1);
	S32 buttonPressedBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1_PRESSED);
	backButton = new Button("Back");
	backButton->userId = -2;
	backButton->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	backButton->setSize(FILL, 60);
	backButton->setMargins(5);
	backButton->setBackgroundMenuResource(buttonBgId);
	backButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	backButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	streamButton = new Button("Start Stream");
	streamButton->userId = -2;
	streamButton->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	streamButton->setSize(FILL, 60);
	streamButton->setMargins(5);
	streamButton->setBackgroundMenuResource(buttonBgId);
	streamButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	streamButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	sound1Button = new Button("Sound 1");
	sound1Button->userId = -2;
	sound1Button->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	sound1Button->setSize(FILL, 60);
	sound1Button->setMargins(5);
	sound1Button->setBackgroundMenuResource(buttonBgId);
	sound1Button->setPressedBackgroundMenuResource(buttonPressedBgId);
	sound1Button->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	sound2Button = new Button("Sound 2");
	sound2Button->userId = -2;
	sound2Button->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	sound2Button->setSize(FILL, 60);
	sound2Button->setMargins(5);
	sound2Button->setBackgroundMenuResource(buttonBgId);
	sound2Button->setPressedBackgroundMenuResource(buttonPressedBgId);
	sound2Button->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	sound3Button = new Button("Sound 3");
	sound3Button->userId = -2;
	sound3Button->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	sound3Button->setSize(FILL, 60);
	sound3Button->setMargins(5);
	sound3Button->setBackgroundMenuResource(buttonBgId);
	sound3Button->setPressedBackgroundMenuResource(buttonPressedBgId);
	sound3Button->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	sound4Button = new Button("Sound 4");
	sound4Button->userId = -2;
	sound4Button->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	sound4Button->setSize(FILL, 60);
	sound4Button->setMargins(5);
	sound4Button->setBackgroundMenuResource(buttonBgId);
	sound4Button->setPressedBackgroundMenuResource(buttonPressedBgId);
	sound4Button->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	titleLabel = new Label("Sound Check");
	titleLabel->setHeight(60);
	titleLabel->setWidth(WRAP);
	titleLabel->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	mainLayout->setHeight(WRAP);
	mainLayout->setWidth(300);
	mainLayout->setBackgroundMenuResource(panelBgId);
	mainLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::VERTICAL_CENTER));
	mainLayout->addComponent(titleLabel);
	mainLayout->addComponent(streamButton);
	mainLayout->addComponent(sound1Button);
	mainLayout->addComponent(sound2Button);
	mainLayout->addComponent(sound3Button);
	mainLayout->addComponent(sound4Button);
	mainLayout->addComponent(backButton);
	setRootComponent(mainLayout);
	isStreaming = FALSE;
	context->audioManager->loadSound("bt_22050.ogg");
	context->audioManager->loadSound("bt_32000.ogg");
	context->audioManager->loadSound("bt_44100.ogg");
	context->audioManager->loadSound("bt_48000.ogg");
}

SoundcheckMenu::~SoundcheckMenu() {
	delete mainLayout;
	mainLayout = NULL;
	delete titleLabel;
	titleLabel = NULL;
	delete backButton;
	backButton = NULL;
	delete streamButton;
	streamButton = NULL;
	delete sound1Button;
	sound1Button = NULL;
	delete sound2Button;
	sound2Button = NULL;
	delete sound3Button;
	sound3Button = NULL;
	delete sound4Button;
	sound4Button = NULL;
}

void SoundcheckMenu::onClickUp(UIComponent *component) {
	if (component == backButton) {
		context->uiManager->popMenu();
		context->uiManager->showMenu(TOP_MENU_NAME);
	} else if (component == streamButton) {
		if (isStreaming) {
			context->audioManager->stopStreamingSound("song1.ogg");
			streamButton->setText("Start Stream");
		} else {
			context->audioManager->playStreamingSound("song1.ogg", -1, 1.0f, 1.0f, 1.0f);
			streamButton->setText("Stop Stream");
		}
		isStreaming = !isStreaming;
	} else if (component == sound1Button) {
		context->audioManager->playSound("bt_22050.ogg", 0, 1.0f, 1.0f, 0.8f);
	} else if (component == sound2Button) {
		context->audioManager->playSound("bt_32000.ogg", 0, 1.0f, 1.0f, 1.0f);
	} else if (component == sound3Button) {
		context->audioManager->playSound("bt_44100.ogg", 0, 1.0f, 1.0f, 1.25f);
	} else if (component == sound4Button) {
		context->audioManager->playSound("bt_48000.ogg", 0, 1.0f, 1.0f, 1.5f);
	}
}

void SoundcheckMenu::onPreShow() {
}

void SoundcheckMenu::setData(void *data) {
}

void SoundcheckMenu::onSpecialKey(SpecialKey sKey) {
	if (sKey == SKEY_BACK) {
		context->uiManager->popMenu();
		context->uiManager->showMenu(TOP_MENU_NAME);
	}
}
