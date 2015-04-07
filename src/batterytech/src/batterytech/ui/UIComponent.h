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

/** \brief A special width or height which specifies that the UIComponent is to attempt to consume all available space */
#define FILL -1
/** \brief A special width or height which specifies that the UIComponent is to only be as large as necessary to show the content */
#define WRAP 0
#define NO_RESOURCE -1
#define NO_ID -1

namespace BatteryTech {

	class UIAnimator;
	class Context;

	/** \brief A User Interface Component
	 * \ingroup UI
	 * \class UIComponent
	 *
	 * This is the foundation and base class of all User Interface Components in the BatteryTech UI Library
	 *
	 * UIComponents may be independent, they may be layouts or they may have any number of child components.
	 */
	class UIComponent {
	public:
		enum HorizontalAlignment { LEFT, HORIZONTAL_CENTER, RIGHT };
		enum VerticalAlignment { TOP, VERTICAL_CENTER, BOTTOM };
		/** \brief Creates a UIComponent
		 * \param text The (optional) label
		 */
		UIComponent(const char *text = NULL);
		virtual ~UIComponent();
		/** \brief Sets the width of this component
		 * \param widthDips The width in reference-scaled pixels, FILL or WRAP
		 */
		virtual void setWidth(S32 widthDips) {this->widthDips = widthDips;};
		/** \brief Sets the height of this component
		 * \param heightDips The height in reference-scaled pixels, FILL or WRAP
		 */
		virtual void setHeight(S32 heightDips) {this->heightDips = heightDips;};
		/** \brief Sets the size of this component
		 * \param widthDips The width in reference-scaled pixels, FILL or WRAP
		 * \param heightDips The height in reference-scaled pixels, FILL or WRAP
		 */
		virtual void setSize(S32 widthDips, S32 heightDips) {this->widthDips = widthDips; this->heightDips = heightDips;};
		/** \brief Sets the padding of this component
		 * \param paddingDips The top, right, bottom and left padding on reference-scaled pixels
		 */
		virtual void setPadding(S32 paddingDips) {this->paddingLeftDips = this->paddingTopDips = this->paddingRightDips = this->paddingBottomDips = paddingDips;};
		/** \brief Sets the margins of this component
		 * \param marginDips The top, right, bottom and left margin on reference-scaled pixels
		 */
		virtual void setMargins(S32 marginDips) {this->marginLeftDips = this->marginTopDips = this->marginRightDips = this->marginBottomDips = marginDips;};
		/** \brief Sets how this component is to be laid out in its parent
		 * \param layoutParameters The parameters for layout
		 */
		virtual void setLayoutParameters(LayoutParameters *layoutParameters) {this->layoutParameters = layoutParameters;};
		/** \brief Sets the optional background menu resource
		 *
		 * If a resource is set here, it will be loaded and drawn as the background for this component.  A resource ID is acquired through
		 * MenuRenderer::addTextureAsset
		 *
		 * \see MenuRenderer::addTextureAsset
		 */
		virtual void setBackgroundMenuResource(S32 menuResourceId) {this->backgroundMenuResourceId = menuResourceId;};
		/** \brief Sets the optional pressed background menu resource
		 *
		 * If a resource is set here, it will be loaded and drawn as the background when this component is in a pressed state.
		 * A resource ID is acquired through MenuRenderer::addTextureAsset
		 *
		 * \see MenuRenderer::addTextureAsset
		 */
		virtual void setPressedBackgroundMenuResource(S32 menuResourceId) {this->pressedBackgroundMenuResourceId = menuResourceId;};
		/** \brief Sets the optional selected background menu resource
		 *
		 * If a resource is set here, it will be loaded and drawn as the background when this component is in a selected state.
		 * A resource ID is acquired through MenuRenderer::addTextureAsset
		 *
		 * \see MenuRenderer::addTextureAsset
		 */
		virtual void setSelectedBackgroundMenuResource(S32 menuResourceId) {this->selectedBackgroundMenuResourceId = menuResourceId;};
		/** \brief Sets the label or text of this component
		 *
		 * \param text The label, may be NULL
		 */
		virtual void setText(const char *text);
		/** \brief Specifies how the text of this component is to be aligned
		 *
		 * \param horizAlignment The horizontal alignment
		 * \param vertAlignment The vertical alignment
		 */
		virtual void setTextAlignment(HorizontalAlignment horizAlignment, VerticalAlignment vertAlignment) {
			this->textHorizontalAlignment = horizAlignment;
			this->textVerticalAlignment = vertAlignment;
		}
		/** \brief Adds a child component to this component
		 *
		 * UIComponents may have up to MAX_UI_SUBCOMPONENTS children.  This function is how you set up a hierarchy of components
		 */
		virtual void addComponent(UIComponent *component);
		// framework methods
		/** \internal */
		virtual S32 getDesiredWidth(Context *context, S32 widthAvailable, S32 heightAvailable) {return widthDips;};
		/** \internal */
		virtual S32 getDesiredHeight(Context *context, S32 widthAvailable, S32 heightAvailable) {return heightDips;};
		/** \internal */
		virtual LayoutParameters* getLayoutParameters() {return layoutParameters;};
		/** \internal */
		virtual void layout(Context *context, F32 scale){};
		// in actual screen coordinates
		/** \internal */
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
		/** \internal */
		virtual void setVirtualBounds(S32 left, S32 top, S32 right, S32 bottom) {
			this->virtualLeft = left;
			this->virtualTop = top;
			this->virtualRight = right;
			this->virtualBottom = bottom;
		}
		/** \internal */
		virtual void scrollAllBounds(S32 x, S32 y) {
			this->left += x;
			this->top += y;
			this->right += x;
			this->bottom += y;
			scrollVirtualBounds(x, y);
		}
		/** \internal */
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
		/** \internal */
		virtual BOOL32 isVirtualLargerThanDrawable() {
			return (right - left < virtualRight - virtualLeft || bottom - top < virtualBottom - virtualTop);
		}
		/** \brief Sets the text color of this component
		 *
		 * \param r The red value 0-1
		 * \param g The green value 0-1
		 * \param b The blue value 0-1
		 * \param a The alpha value 0-1
		 */
		virtual void setTextColor(F32 r, F32 g, F32 b, F32 a) {
			textR = r;
			textG = g;
			textB = b;
			textA = a;
		}
		/** \internal */
		virtual void enter();
		/** \internal */
		virtual void exit();
		/** \brief will be called if this component becomes selected */
		virtual void onSelected(){};
		/** \internal */
		virtual BOOL32 isEnterPending() { return isEntering; };
		/** \internal */
		virtual BOOL32 isExitPending() { return (isExiting || removeFromView); };
		/** \internal */
		virtual BOOL32 isExitDone();
		/** \brief Will be called when system updates */
		virtual void update(F32 delta);
		/** \internal */
		virtual void dispatchClickDown(S32 x, S32 y);
		/** \internal */
		virtual void dispatchClickMove(S32 x, S32 y);
		/** \internal */
		virtual void dispatchClickUp();
		/** \internal */
		virtual BOOL32 dispatchKeyPressed(U8 key);
		/** \internal */
		virtual BOOL32 dispatchKeyDown(U8 key);
		/** \internal */
		virtual BOOL32 dispatchKeyUp(U8 key);
		/** \brief Sets the enter animator for this component
		 *
		 * \param animator The animator to run for the component entering the scene
		 */
		virtual void setEnterAnimator(UIAnimator *animator);
		/** \brief Sets the main animator for this component
		 *
		 * \param animator The animator to run after the component has entered and before the component has exited
		 */
		virtual void setMainAnimator(UIAnimator *animator);
		/** \brief Sets the exit animator for this component
		 *
		 * \param animator The animator to run just before the component exits
		 */
		virtual void setExitAnimator(UIAnimator *animator);
		/** \private */
		virtual UIAnimator* getActiveAnimator();
		/** \brief Sets the sound asset to play when the component is clicked down on */
		virtual void setClickDownSoundAsset(const char *assetName);
		/** \brief Sets the sound asset to play when the component is clicked up on */
		virtual void setClickUpSoundAsset(const char *assetName);
		/** \brief Sets the vibration effect to play when the component is clicked down on */
		virtual void setClickDownVibeEffectId(S32 vibeEffectId);
		/** \brief Sets the vibration effect to play when the component is clicked up on */
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
		BOOL32 isHidden; 
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
		/** Return true if the UIComponent consumed the key press */
		virtual BOOL32 onKeyPressed(U8 key){return FALSE;};
		virtual BOOL32 onKeyDown(U8 key){return FALSE;};
		virtual BOOL32 onKeyUp(U8 key){return FALSE;};
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
