/*
 * Layout.h
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "UIComponent.h"
#include "../util/ManagedArray.h"

class Layout: public UIComponent {
public:
	Layout();
	virtual void addComponent(UIComponent *component);
	virtual ~Layout();
protected:
	ManagedArray<UIComponent> *components;
};

#endif /* LAYOUT_H_ */
