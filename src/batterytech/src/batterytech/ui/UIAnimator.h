/*
 * UIAnimator.h
 *
 *  Created on: Oct 11, 2010
 *      Author: rgreen
 */

#ifndef UIANIMATOR_H_
#define UIANIMATOR_H_

#include "../primitives.h"
#include "../render/GraphicsConfiguration.h"
#include "UIComponent.h"

//cross-ref
class UIComponent;

class UIAnimator {
public:
	UIAnimator();
	virtual ~UIAnimator();
	virtual void reset(){};
	virtual void update(F32 delta){};
	virtual void drawPreComponent(GraphicsConfiguration *gConfig){};
	virtual void drawPostComponent(GraphicsConfiguration *gConfig){};
	virtual void setComponent(UIComponent *component){ this->component = component; };
	virtual BOOL32 isComplete(){return TRUE;};
protected:
	UIComponent *component;
};

#endif /* UIANIMATOR_H_ */
