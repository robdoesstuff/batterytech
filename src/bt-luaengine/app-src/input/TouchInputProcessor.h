/*
 * TouchInputProcessor.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rgreen
 */

#ifndef TOUCHINPUTPROCESSOR_H_
#define TOUCHINPUTPROCESSOR_H_

#include <batterytech/primitives.h>

#include "../GameContext.h"

using namespace BatteryTech;

class ScreenControl;

class TouchInputProcessor {
public:
	TouchInputProcessor();
	virtual ~TouchInputProcessor();
	void processInput(GameContext *context);
	void notifyScreenControlRemoved(ScreenControl *screenControl);
private:
	BOOL32 *lastTouchDown;
	ScreenControl **touchedControl;
};

#endif /* TOUCHINPUTPROCESSOR_H_ */
