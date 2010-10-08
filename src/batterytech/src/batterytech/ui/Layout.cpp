/*
 * Layout.cpp
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#include "Layout.h"
#include "../util/ManagedArray.h"

Layout::Layout() : UIComponent() {
	components = new ManagedArray<UIComponent>(10);
}

void Layout::addComponent(UIComponent *component) {
	components->add(component);
}

Layout::~Layout() {
	delete components;
}
