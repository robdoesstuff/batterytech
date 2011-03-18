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
// Name        : UIComponent.h
// Description : The base UI class that all other UIComponents extend and build from.  Contains all common functionality.
//============================================================================

#ifndef UICOMPONENT_H_
#define UICOMPONENT_H_

#include "../primitives.h"
#include "LayoutParameters.h"
#include "../Logger.h"
#include <stdio.h>
#include "../util/ManagedArray.h"
#include "UIAnimator.h"

#define FILL -1
#define WRAP 0
#define NO_RESOURCE -1
#define NO_ID -1

namespace BatteryTech {

	class UIAnimator;
	class Context;

	class UIComponent {
	public:
		enum HorizontalAlignment { LEFT, HORIZONTAL_CENTER, RIGHT };
		enum VerticalAlignment { TOP, VERTICAL_CENTER, BOTTOM };
		UIComponent(const char *text = NULL);
		virtual ~UIComponent();
		// user methods, default unit is density-independent pixels
		virtual void setWidth(S32 widthDips) {this->widthDips = widthDips;};
		virtual void setHeight(S32 heightDips) {this->heightDips = heightDips;};
		virtual void setSize(S32 widthDips, S32 heightDips) {this->widthDips = widthDips; this->heightDips = heightDips;};
		virtual void setPadding(S32 paddingDips) {this->paddingLeftDips = this->paddingTopDips = this->paddingRightDips = this->paddingBottomDips = paddingDips;};
		virtual void setMargins(S32 marginDips) {this->marginLeftDips = this->marginTopDips = this->marginRightDips = this->marginBottomDips = marginDips;};
		virtual void setLayoutParameters(LayoutParameters *layoutParameters) {this->layoutParameters = layoutParameters;};
		virtual void setBackgroundMenuResource(S32 menuResourceId) {this->backgroundMenuResourceId = menuResourceId;};
		virtual void setPressedBackgroundMenuResource(S32 menuResourceId) {this->pressedBackgroundMenuResourceId = menuResourceId;};
		virtual void setSelectedBackgroundMenuResource(S32 menuResourceId) {this->selectedBackgroundMenuResourceId = menuResourceId;};
		virtual void setText(const char *text);
		virtual void setTextAlignment(HorizontalAlignment horizAlignment, VerticalAlignment vertAlignment) {
			this->textHorizontalAlignment = horizAlignment;
			this->textVerticalAlignment = vertAlignment;
		}
		virtual void addComponent(UIComponent *component);
		// framework methods
		virtual S32 getDesiredWidth(Context *context, S32 widthAvailable, S32 heightAvailable) {return widthDips;};
		virtual S32 getDesiredHeight(Context *context, S32 widthAvailable, S32 heightAvailable) {return heightDips;};
		virtual LayoutParameters* getLayoutParameters() {return layoutParameters;};
		virtual void layout(Context *context, F32 scale){};
		// in actual screen coordinates
		virtual void setDrawableBounds(S32 left, S32 top, S32 right, S32 bottom) {
			//char buf[100];
			if (text) {
				//sprintf(buf, "Component %s Bounds set to [%d, %d, %d, %d]", text, left, top, right, bottom);
			} else {
				//sprintf(buf, "Component '' Bounds set to [%d, %d, %d, %d]", left, top, right, bottom);
			}
			//logmsg(buf);
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
			setVirtualBounds(left, top, right, bottom);
		}
		virtual void setVirtualBounds(S32 left, S32 top, S32 right, S32 bottom) {
			this->virtualLeft = left;
			this->virtualTop = top;
			this->virtualRight = right;
			this->virtualBottom = bottom;
		}
		virtual void scrollAllBounds(S32 x, S32 y) {
			this->left += x;
			this->top += y;
			this->right += x;
			this->bottom += y;
			scrollVirtualBounds(x, y);
		}
		virtual void scrollVirtualBounds(S32 x, S32 y) {
			this->virtualLeft += x;
			this->virtualTop += y;
			this->virtualRight+= x;
			this->virtualBottom += y;
			if (components) {
				for (S32 i = 0; i < components->getSize(); i++) {
					// all subcomponents and theirs will also need to scroll drawable bounds
					components->array[i]->scrollAllBounds(x, y);
				}
			}
		}
		virtual BOOL32 isVirtualLargerThanDrawable() {
			return (right - left < virtualRight - virtualLeft || bottom - top < virtualBottom - virtualTop);
		}
		virtual void setTextColor(F32 r, F32 g, F32 b, F32 a) {
			textR = r;
			textG = g;
			textB = b;
			textA = a;
		}
		virtual void enter();
		virtual void exit();
		virtual void onSelected(){};
		virtual BOOL32 isEnterPending() { return isEntering; };
		virtual BOOL32 isExitPending() { return (isExiting || removeFromView); };
		virtual BOOL32 isExitDone();
		virtual void update(F32 delta);
		virtual void dispatchClickDown(S32 x, S32 y);
		virtual void dispatchClickMove(S32 x, S32 y);
		virtual void dispatchClickUp();
		virtual void dispatchKeyPressed(U8 key);
		virtual void setEnterAnimator(UIAnimator *animator);
		virtual void setMainAnimator(UIAnimator *animator);
		virtual void setExitAnimator(UIAnimator *animator);
		virtual UIAnimator* getActiveAnimator();
		virtual void setClickDownSoundAsset(const char *assetName);
		virtual void setClickUpSoundAsset(const char *assetName);
		virtual void setClickDownVibeEffectId(S32 vibeEffectId);
		virtual void setClickUpVibeEffectId(S32 vibeEffectId);
		S32 paddingLeftDips, paddingTopDips, paddingRightDips, paddingBottomDips;
		S32 marginLeftDips, marginTopDips, marginRightDips, marginBottomDips;
		S32 backgroundMenuResourceId;
		S32 pressedBackgroundMenuResourceId;
		S32 selectedBackgroundMenuResourceId;
		char *text;
		BOOL32 isTextMultiline;
		S32 left, top, right, bottom;
		S32 virtualLeft, virtualTop, virtualRight, virtualBottom;
		F32 textR, textB, textG, textA;
		HorizontalAlignment textHorizontalAlignment;
		VerticalAlignment textVerticalAlignment;
		ManagedArray<UIComponent> *components;
		BOOL32 isClickable;
		BOOL32 isSelectable;
		BOOL32 isEnabled;
		BOOL32 isPressed;
		BOOL32 removeFromView;
		BOOL32 isClickableUnderChildren;
		S32 userId;
		const char *clickDownSoundAsset;
		const char *clickUpSoundAsset;
		BOOL32 playClickDownSound;
		BOOL32 playClickUpSound;
		S32 clickDownVibeEffectId;
		S32 clickUpVibeEffectId;
		BOOL32 playClickDownVibeEffect;
		BOOL32 playClickUpVibeEffect;
	protected:
		S32 widthDips;
		S32 heightDips;
		virtual void onClickDown(S32 x, S32 y){};
		virtual void onClickUp(){};
		virtual void onClickMove(S32 x, S32 y){};
		virtual void onKeyPressed(U8 key){};
		LayoutParameters *layoutParameters;
	private:
		UIAnimator *enterAnimator;
		UIAnimator *mainAnimator;
		UIAnimator *exitAnimator;
		BOOL32 isEntering;
		BOOL32 isExiting;
	};

}
#endif /* UICOMPONENT_H_ */
