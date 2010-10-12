/*
 * MainMenu.cpp
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#include "OptionsMenu.h"
#include "MainMenu.h"
#include "../primitives.h"
#include "../ui/LinearLayout.h"
#include "../ui/Button.h"
#include "../ui/SlideAnimator.h"

OptionsMenu::OptionsMenu(Context *context) : Menu(OPTIONS_MENU_NAME) {
	this->context = context;
	S32 buttonBgId = context->menuRenderer->addTextureAsset("button1_tex.png");
	S32 buttonPressedBgId = context->menuRenderer->addTextureAsset("button1_pressed_tex.png");
	S32 panelBgId = context->menuRenderer->addTextureAsset("panel2_tex.png");
	LinearLayout *buttonLayout = new LinearLayout(LinearLayout::HORIZONTAL);
	Button *button1 = new Button("Button 1");
	button1->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::BOTTOM));
	button1->setSize(FILL, 60);
	button1->setMargins(5);
	button1->setBackgroundMenuResource(buttonBgId);
	button1->setPressedBackgroundMenuResource(buttonPressedBgId);
	Button *button2 = new Button("Button 2");
	button2->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::BOTTOM));
	button2->setSize(FILL, 60);
	button2->setMargins(5);
	button2->setBackgroundMenuResource(buttonBgId);
	button2->setPressedBackgroundMenuResource(buttonPressedBgId);
	Button *button3 = new Button("Button 3");
	button3->setLayoutParameters(new LayoutParameters(LayoutParameters::LEFT, LayoutParameters::BOTTOM));
	button3->setSize(FILL, 60);
	button3->setMargins(5);
	button3->setBackgroundMenuResource(buttonBgId);
	button3->setPressedBackgroundMenuResource(buttonPressedBgId);
	buttonLayout->addComponent(button1);
	buttonLayout->addComponent(button2);
	buttonLayout->addComponent(button3);
	buttonLayout->setHeight(WRAP);
	buttonLayout->setWidth(FILL);
	buttonLayout->setBackgroundMenuResource(panelBgId);
	buttonLayout->setLayoutParameters(new LayoutParameters(LayoutParameters::HORIZONTAL_CENTER, LayoutParameters::BOTTOM));
	buttonLayout->setEnterAnimator(new SlideAnimator(SlideAnimator::BOTTOM, SlideAnimator::IN, 0.5f));
	buttonLayout->setExitAnimator(new SlideAnimator(SlideAnimator::BOTTOM, SlideAnimator::OUT, 0.5f));
	setRootComponent(buttonLayout);
}

void OptionsMenu::onClickDown(UIComponent *component){
}

void OptionsMenu::onClickUp(UIComponent *component){
	this->context->uiManager->popMenu();
	this->context->uiManager->showMenu(MAIN_MENU_NAME);
}

OptionsMenu::~OptionsMenu() {
	this->context = NULL;
}
