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

#include "ErrorMenu.h"
#include "../UIConstants.h"
#include "../GameConstants.h"
#include <batterytech/audio/AudioManager.h>
#include <batterytech/VibrationManager.h>
#include <batterytech/util/PropertiesIO.h>
#include <batterytech/platform/platformgeneral.h>
#include <string.h>
#include "../level/LevelIO.h"
#include "../Game.h"

#define ERROR_BG "ui/panel1_tex.png"
#define DEFAULT_HEIGHT 675
#define DEFAULT_WIDTH 1000

ErrorMenu::ErrorMenu(GameContext *context) : Menu(ERROR_MENU_NAME) {
	this->context = context;
	S32 panelBgId = context->menuRenderer->addTextureAsset(ERROR_BG);
	mainLayout = new LinearLayout(LinearLayout::VERTICAL);
	S32 buttonBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1);
	S32 buttonPressedBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1_PRESSED);
	okButton = new Button("Ok");
	okButton->userId = 1;
	okButton->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	okButton->setSize(240, 120);
	okButton->setMargins(10);
	okButton->setBackgroundMenuResource(buttonBgId);
	okButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	okButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	restartButton = new Button("Restart");
	restartButton->userId = 2;
	restartButton->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	restartButton->setSize(240, 120);
	restartButton->setMargins(10);
	restartButton->setBackgroundMenuResource(buttonBgId);
	restartButton->setPressedBackgroundMenuResource(buttonPressedBgId);
	restartButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	titleLabel = new Label("Title");
	titleLabel->setHeight(70);
	titleLabel->setMargins(15);
	titleLabel->setWidth(FILL);
	titleLabel->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	titleLabel->setTextAlignment(UIComponent::HORIZONTAL_CENTER, UIComponent::TOP);
	errorMessage = new Label("ErrorMessage");
	errorMessage->setHeight(WRAP);
	errorMessage->setWidth(FILL);
	errorMessage->setPadding(20);
	errorMessage->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	mainLayout->setHeight(WRAP);
	mainLayout->setWidth(DEFAULT_WIDTH);
	mainLayout->setBackgroundMenuResource(panelBgId);
	mainLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::VERTICAL_CENTER));
	mainLayout->addComponent(titleLabel);
	mainLayout->addComponent(errorMessage);
	mainLayout->addComponent(okButton);
	mainLayout->addComponent(restartButton);
	setRootComponent(mainLayout);
}

ErrorMenu::~ErrorMenu() {
	delete mainLayout;
	mainLayout = NULL;
	delete titleLabel;
	titleLabel = NULL;
	delete errorMessage;
	errorMessage = NULL;
	delete okButton;
	okButton = NULL;
	delete restartButton;
	restartButton = NULL;
}

void ErrorMenu::onClickUp(UIComponent *component) {
	if (component == okButton) {
		context->game->clearError();
		context->uiManager->popMenu();
	} else if (component == restartButton) {
		context->game->clearError();
		context->game->reset();
	}
}

void ErrorMenu::onPreShow() {
	layoutRequested = TRUE;
}

void ErrorMenu::setData(void *data) {
	if (data) {
		ErrorMessage *em = (ErrorMessage*)data;
		titleLabel->setText(em->title);
		errorMessage->setText(em->message);
		mainLayout->components->remove(this->restartButton);
		if(em->showRestart) {
			mainLayout->addComponent(restartButton);
		} 
		if (em->width != 0) {
			mainLayout->setWidth(em->width);
		} else {
			mainLayout->setWidth(DEFAULT_WIDTH);
		}
		requestLayout();
		delete em;
	}
}

void ErrorMenu::onSpecialKey(SpecialKey sKey) {
}
