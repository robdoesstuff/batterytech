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
#include "SettingsMenu.h"
#include "../Game.h"
#include "ErrorMenu.h"
#include <batterytech/ui/SlideAnimator.h>

#define RACE_BUTTON "ui/main_menu/menu_button_race.png"
#define RACE_BUTTON_PRESSED "ui/main_menu/menu_button_race_pressed.png"
#define GARAGE_BUTTON "ui/main_menu/menu_button_garage.png"
#define GARAGE_BUTTON_PRESSED "ui/main_menu/menu_button_garage_pressed.png"
#define SETTINGS_BUTTON "ui/main_menu/menu_button_settings.png"
#define SETTINGS_BUTTON_PRESSED "ui/main_menu/menu_button_settings_pressed.png"
#define OF_BUTTON "ui/main_menu/menu_button_of.png"
#define OF_BUTTON_PRESSED "ui/main_menu/menu_button_of_pressed.png"

TopMenu::TopMenu(GameContext *context) : Menu(TOP_MENU_NAME) {
	this->context = context;
	mainLayout = new LinearLayout(LinearLayout::HORIZONTAL);
	/*
	S32 panelBgId = context->menuRenderer->addTextureAsset(UI_PANEL_2);
	S32 buttonBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1);
	S32 buttonPressedBgId = context->menuRenderer->addTextureAsset(UI_BUTTON_1_PRESSED);
	subLayout1 = new LinearLayout(LinearLayout::VERTICAL);
	subLayout2 = new LinearLayout(LinearLayout::VERTICAL);
	
	titleLabel = new Label("NSCRA");
	titleLabel->setHeight(60);
	titleLabel->setWidth(WRAP);
	titleLabel->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	raceButton = new Button("Race");
	raceButton->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::TOP));
	raceButton->setSize(300, 60);
	raceButton->setMargins(5);
	raceButton->paddingRightDips = 30;
	raceButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	raceButton->setBackgroundMenuResource(context->menuRenderer->addTextureAsset(RACE_BUTTON));
	raceButton->setPressedBackgroundMenuResource(context->menuRenderer->addTextureAsset(RACE_BUTTON_PRESSED));
	raceButton->setTextAlignment(UIComponent::RIGHT, UIComponent::VERTICAL_CENTER);
	garageButton = new Button("Garage");
	garageButton->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::TOP));
	garageButton->setSize(300, 60);
	garageButton->setMargins(5);
	garageButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	garageButton->setBackgroundMenuResource(context->menuRenderer->addTextureAsset(GARAGE_BUTTON));
	garageButton->setPressedBackgroundMenuResource(context->menuRenderer->addTextureAsset(GARAGE_BUTTON_PRESSED));
	garageButton->setTextAlignment(UIComponent::RIGHT, UIComponent::VERTICAL_CENTER);
	garageButton->paddingRightDips = 30;
	settingsButton = new Button("Settings");
	settingsButton->setLayoutParameters(new LayoutParameters(LayoutParameters::RIGHT, LayoutParameters::TOP));
	settingsButton->setSize(300, 60);
	settingsButton->setMargins(5);
	settingsButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	settingsButton->setBackgroundMenuResource(context->menuRenderer->addTextureAsset(SETTINGS_BUTTON));
	settingsButton->setPressedBackgroundMenuResource(context->menuRenderer->addTextureAsset(SETTINGS_BUTTON_PRESSED));
	settingsButton->setTextAlignment(UIComponent::LEFT, UIComponent::VERTICAL_CENTER);
	settingsButton->paddingLeftDips = 30;
	leaderboardButton = new Button("Times");
	leaderboardButton->setLayoutParameters(new LayoutParameters(LayoutParameters::RIGHT, LayoutParameters::TOP));
	leaderboardButton->setSize(300, 60);
	leaderboardButton->setMargins(5);
	leaderboardButton->setTextAlignment(UIComponent::LEFT, UIComponent::VERTICAL_CENTER);
	leaderboardButton->setBackgroundMenuResource(context->menuRenderer->addTextureAsset(OF_BUTTON));
	leaderboardButton->setPressedBackgroundMenuResource(context->menuRenderer->addTextureAsset(OF_BUTTON_PRESSED));
	leaderboardButton->setClickDownSoundAsset(UI_CLICK_DOWN_ASSETNAME);
	leaderboardButton->paddingLeftDips = 30;
	
    S32 subLayoutWidth = 360;
    if (subLayoutWidth * 2 * context->gConfig->uiScale > context->gConfig->viewportWidth) {
        subLayoutWidth = 320;
    }
	subLayout1->setHeight(150);
	subLayout1->setWidth(subLayoutWidth);
	subLayout1->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::VERTICAL_CENTER));
	subLayout1->addComponent(raceButton);
	subLayout1->addComponent(garageButton);
	subLayout1->marginTopDips = 90;
	subLayout1->setEnterAnimator(new SlideAnimator(SlideAnimator::LEFT, SlideAnimator::SLIDE_IN, 0.5f));
	subLayout1->setExitAnimator(new SlideAnimator(SlideAnimator::LEFT, SlideAnimator::SLIDE_OUT, 0.5f));
	
	subLayout2->setHeight(150);
	subLayout2->setWidth(subLayoutWidth);
	subLayout2->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::VERTICAL_CENTER));
	subLayout2->addComponent(leaderboardButton);
	subLayout2->addComponent(settingsButton);
	subLayout2->marginTopDips = 90;
	subLayout2->setEnterAnimator(new SlideAnimator(SlideAnimator::RIGHT, SlideAnimator::SLIDE_IN, 0.5f));
	subLayout2->setExitAnimator(new SlideAnimator(SlideAnimator::RIGHT, SlideAnimator::SLIDE_OUT, 0.5f));
	
	mainLayout->addComponent(subLayout1);
	mainLayout->addComponent(subLayout2);
	
	*/
	mainLayout->setHeight(WRAP);
	mainLayout->setWidth(FILL);
	mainLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::VERTICAL_CENTER));
	setRootComponent(mainLayout);
}

TopMenu::~TopMenu() {
	delete mainLayout;
	mainLayout = NULL;
	/*
	delete titleLabel;
	titleLabel = NULL;
	delete raceButton;
	raceButton = NULL;
	delete garageButton;
	garageButton = NULL;
	delete settingsButton;
	settingsButton = NULL;
	delete leaderboardButton;
	leaderboardButton = NULL;
	*/
}

void TopMenu::onClickUp(UIComponent *component) {
	/*
	if (component == raceButton) {
		const char* selCar = this->context->game->luaBinder->getStringFromGlobalTable("carinventory", "SelectedCar");
		if(selCar != NULL && strlen(selCar) != 0) {
			context->uiManager->popMenu();
			// set the running state first in case the race mode needs to flip to loading
			context->world->gameState = GAMESTATE_RUNNING;
			context->game->luaBinder->setMode(GAMEMODE_RACE);
		} else {
			context->uiManager->showMenu(ERROR_MENU_NAME, 
				new ErrorMessage("No Car Selected", "Visit the garage to buy and select a car", false, 320));
		}
	} else if (component == garageButton) {
		context->uiManager->popMenu();
		context->world->gameState = GAMESTATE_RUNNING;
		context->game->luaBinder->setMode(GAMEMODE_GARAGE);
	} else if (component == settingsButton) {
		context->uiManager->popMenu();
		context->uiManager->showMenu(SETTINGS_MENU_NAME, (void*)TOP_MENU_NAME);
	} else if (component == leaderboardButton) {
		_platform_hook("showLeaderboards", NULL, 0);
	}
	*/
}

void TopMenu::onPreShow() {
}

void TopMenu::onSpecialKey(SpecialKey sKey) {
	if (sKey == SKEY_BACK) {
		_platform_exit();
	}
}
