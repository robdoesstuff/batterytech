/*
 * LinearLayout.cpp
 *
 *  Created on: Sep 24, 2010
 *      Author: rgreen
 */

#include "LinearLayout.h"
#include "../logger.h"
#include <stdio.h>

LinearLayout::LinearLayout() : Layout() {
}

LinearLayout::~LinearLayout() {
}

void LinearLayout::layout() {
	char buf[32];
	sprintf(buf, "Laying out %d UI Components", components->getSize());
	log(buf);
}

void LinearLayout::setDrawableBounds(S32 left, S32 top, S32 right, S32 bottom) {

}

void LinearLayout::update() {

}

void LinearLayout::draw() {

}
