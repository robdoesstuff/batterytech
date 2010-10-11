/*
 * MainMenu.cpp
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#include "MainMenu.h"
#include "OptionsMenu.h"
#include "../primitives.h"
#include "../ui/LinearLayout.h"
#include "../ui/Button.h"

MainMenu::MainMenu(Context *context) : Menu(MAIN_MENU_NAME) {
	this->context = context;
	S32 buttonBgId = context->menuRenderer->addTextureAsset("button1_tex.png");
	S32 buttonPressedBgId = context->menuRenderer->addTextureAsset("button1_pressed_tex.png");
	S32 panelBgId = context->menuRenderer->addTextureAsset("panel2_tex.png");
	LinearLayout *buttonLayout = new LinearLayout(LinearLayout::VERTICAL);
	Button *button1 = new Button("Button 1");
	button1->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	button1->setSize(160, 60);
	button1->setMargins(5);
	button1->setBackgroundMenuResource(buttonBgId);
	button1->setPressedBackgroundMenuResource(buttonPressedBgId);
	Button *button2 = new Button("Button 2");
	button2->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	button2->setSize(160, 60);
	button2->setMargins(5);
	button2->setBackgroundMenuResource(buttonBgId);
	button2->setPressedBackgroundMenuResource(buttonPressedBgId);
	Button *button3 = new Button("Button 3");
	button3->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	button3->setSize(160, 60);
	button3->setMargins(5);
	button3->setBackgroundMenuResource(buttonBgId);
	button3->setPressedBackgroundMenuResource(buttonPressedBgId);
	buttonLayout->addComponent(button1);
	buttonLayout->addComponent(button2);
	buttonLayout->addComponent(button3);
	buttonLayout->setHeight(FILL);
	buttonLayout->setWidth(WRAP);
	buttonLayout->setBackgroundMenuResource(panelBgId);
	buttonLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::TOP));
	LinearLayout *bottomLayout = new LinearLayout(LinearLayout::HORIZONTAL);
	Button *button4 = new Button("A");
	button4->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	button4->setSize(90, 60);
	button4->setMargins(5);
	button4->setBackgroundMenuResource(buttonBgId);
	button4->setPressedBackgroundMenuResource(buttonPressedBgId);
	Button *button5 = new Button("B");
	button5->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::TOP));
	button5->setSize(90, 60);
	button5->setMargins(5);
	button5->setBackgroundMenuResource(buttonBgId);
	button5->setPressedBackgroundMenuResource(buttonPressedBgId);
	bottomLayout->addComponent(button4);
	bottomLayout->addComponent(button5);
	bottomLayout->setWidth(WRAP);
	bottomLayout->setHeight(WRAP);
	bottomLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::BOTTOM));
	buttonLayout->addComponent(bottomLayout);
	setRootComponent(buttonLayout);
}

void MainMenu::onClickDown(UIComponent *component){
}

void MainMenu::onClickUp(UIComponent *component){
	this->context->uiManager->popMenu();
	this->context->uiManager->showMenu(OPTIONS_MENU_NAME);
}

MainMenu::~MainMenu() {
	this->context = NULL;
}
