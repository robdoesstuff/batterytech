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

//============================================================================
// Name        : UIComponent.cpp
// Description : The base UI class that all other UIComponents extend and build from.  Contains all common functionality.
//============================================================================

#include "UIComponent.h"
#include <string.h>
#include "../util/strx.h"
#include "../batterytech_globals.h"

namespace BatteryTech {

	UIComponent::UIComponent(const char *text) {
		widthDips = 0;
		heightDips = 0;
		paddingLeftDips = paddingTopDips = paddingRightDips = paddingBottomDips = 0;
		marginBottomDips = 0;
		marginLeftDips = 0;
		marginRightDips = 0;
		marginTopDips = 0;
		left = top = right = bottom = 0;
		virtualLeft = virtualTop = virtualRight = virtualBottom = 0;
		layoutParameters = NULL;
		this->isHidden = false;
		backgroundMenuResourceId = NO_RESOURCE;
		pressedBackgroundMenuResourceId = NO_RESOURCE;
		selectedBackgroundMenuResourceId = NO_RESOURCE;
		// use a copy of the text.
		this->text = strDuplicate(text);
		isTextMultiline = FALSE;
		textHorizontalAlignment = HORIZONTAL_CENTER;
		textVerticalAlignment = VERTICAL_CENTER;
		components = new ManagedArray<UIComponent> (MAX_UI_SUBCOMPONENTS);
		isClickable = TRUE;
		isClickableUnderChildren = FALSE;
		isSelectable = TRUE;
		isEnabled = TRUE;
		isPressed = FALSE;
		isEntering = FALSE;
		isExiting = FALSE;
		removeFromView = FALSE;
		enterAnimator = NULL;
		mainAnimator = NULL;
		exitAnimator = NULL;
		userId = NO_ID;
		textR = textB = textG = textA = 1.0f;
		clickDownSoundAsset = NULL;
		clickUpSoundAsset = NULL;
		playClickDownSound = FALSE;
		playClickUpSound = FALSE;
		clickDownVibeEffectId = -1;
		clickUpVibeEffectId = -1;
		playClickDownVibeEffect = FALSE;
		playClickUpVibeEffect = FALSE;
	}


	UIComponent::~UIComponent() {
		delete enterAnimator;
		delete mainAnimator;
		delete exitAnimator;
		delete layoutParameters;
		delete components;
		delete [] text;
		if (clickDownSoundAsset) {
			delete [] clickDownSoundAsset;
		}
		if (clickUpSoundAsset) {
			delete [] clickUpSoundAsset;
		}
	}

	void UIComponent::setText(const char *text) {
		// use a copy of the text, delete previous if exists
		if (this->text) {
			delete [] this->text;
		}
		this->text = strDuplicate(text);
	}

	void UIComponent::addComponent(UIComponent *component) {
		components->add(component);
	}

	void UIComponent::dispatchClickDown(S32 x, S32 y) {
		if (clickDownSoundAsset) {
			playClickDownSound = TRUE;
		}
		if (clickDownVibeEffectId != -1) {
			playClickDownVibeEffect = TRUE;
		}
		onClickDown(x, y);
	}

	void UIComponent::dispatchClickMove(S32 x, S32 y) {
		onClickMove(x, y);
		//logmsg("ClickMove");
	}

	void UIComponent::dispatchClickUp() {
		if (clickUpSoundAsset) {
			playClickUpSound = TRUE;
		}
		if (clickUpVibeEffectId != -1) {
			playClickUpVibeEffect = TRUE;
		}
		onClickUp();
	}

	BOOL32 UIComponent::dispatchKeyPressed(U8 key) {
		return onKeyPressed(key);
	}

	BOOL32 UIComponent::dispatchKeyDown(U8 key) {
		return onKeyDown(key);
	}

	BOOL32 UIComponent::dispatchKeyUp(U8 key) {
		return onKeyUp(key);
	}

	void UIComponent::update(F32 delta) {
		if (isEntering && enterAnimator) {
			enterAnimator->update(delta);
			if (enterAnimator->isComplete()) {
				isEntering = FALSE;
				if (mainAnimator) {
					mainAnimator->reset();
				}
			}
		} else if (mainAnimator && !isEntering && !isExiting) {
			mainAnimator->update(delta);
		} else if (isExiting && exitAnimator) {
			exitAnimator->update(delta);
			if (exitAnimator->isComplete()) {
				isExiting = FALSE;
				removeFromView = TRUE;
			}
		}
	}

	void UIComponent::enter() {
		removeFromView = FALSE;
		isExiting = FALSE;
		if (enterAnimator) {
			enterAnimator->reset();
			isEntering = TRUE;
		} else {
			isEntering = FALSE;
		}
		if (components) {
			S32 i;
			for (i = 0; i < components->getSize(); i++) {
				UIComponent *subComp = components->array[i];
				subComp->enter();
			}
		}

	}

	void UIComponent::exit() {
		isEntering = FALSE;
		if (exitAnimator) {
			exitAnimator->reset();
			isExiting = TRUE;
		} else {
			isExiting = FALSE;
			removeFromView = TRUE;
		}
		if (components) {
			S32 i;
			for (i = 0; i < components->getSize(); i++) {
				UIComponent *subComp = components->array[i];
				subComp->exit();
			}
		}
	}

	BOOL32 UIComponent::isExitDone() {
		if (removeFromView) {
			if (components) {
				S32 i;
				for (i = 0; i < components->getSize(); i++) {
					UIComponent *subComp = components->array[i];
					if (!subComp->isExitDone()) {
						return FALSE;
					}
				}
			}
			return TRUE;
		}
		return FALSE;
	}

	UIAnimator* UIComponent::getActiveAnimator() {
		if (isEntering && enterAnimator) {
			return enterAnimator;
		} else if ((isExiting || removeFromView) && exitAnimator) {
			return exitAnimator;
		} else if (mainAnimator) {
			return mainAnimator;
		}
		return NULL;
	}

	void UIComponent::setEnterAnimator(UIAnimator *animator) {
		this->enterAnimator = animator;
		if (animator) {
			animator->setComponent(this);
		}
	}

	void UIComponent::setMainAnimator(UIAnimator *animator) {
		this->mainAnimator = animator;
		if (animator) {
			animator->setComponent(this);
		}
	}

	void UIComponent::setExitAnimator(UIAnimator *animator) {
		this->exitAnimator = animator;
		if (animator) {
			animator->setComponent(this);
		}
	}

	void UIComponent::setClickDownSoundAsset(const char *assetName) {
		// use a copy of the text, delete previous if exists
		if (this->clickDownSoundAsset) {
			delete [] this->clickDownSoundAsset;
		}
		this->clickDownSoundAsset = strDuplicate(assetName);
	}

	void UIComponent::setClickUpSoundAsset(const char *assetName) {
		// use a copy of the text, delete previous if exists
		if (this->clickUpSoundAsset) {
			delete [] this->clickUpSoundAsset;
		}
		this->clickUpSoundAsset = strDuplicate(assetName);
	}

	void UIComponent::setClickDownVibeEffectId(S32 vibeEffectId) {
		clickDownVibeEffectId = vibeEffectId;
	}

	void UIComponent::setClickUpVibeEffectId(S32 vibeEffectId) {
		clickUpVibeEffectId = vibeEffectId;
	}

}
