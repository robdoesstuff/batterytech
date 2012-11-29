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
// Name        : UIAnimator.h
// Description : Abstract base class for UI Animators.
// Usage       : Subclass with specific functionality.
//============================================================================

#ifndef UIANIMATOR_H_
#define UIANIMATOR_H_

#include "../primitives.h"
#include "../render/GraphicsConfiguration.h"
#include "UIComponent.h"

namespace BatteryTech {

	class UIComponent;
	class Context;

	/** \brief UIAnimator Base class for bolt-on animator components
	 * \ingroup UI
	 * \class UIAnimator
	 */
	class UIAnimator {
	public:
		UIAnimator();
		virtual ~UIAnimator();
		virtual void reset(){};
		virtual void update(F32 delta){};
		virtual void drawPreComponent(Context *context){};
		virtual void drawPostComponent(Context *context){};
		virtual void setComponent(UIComponent *component){ this->component = component; };
		virtual BOOL32 isComplete(){return TRUE;};
	protected:
		UIComponent *component;
	};

}
#endif /* UIANIMATOR_H_ */
