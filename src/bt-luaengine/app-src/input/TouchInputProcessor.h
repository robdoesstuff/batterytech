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
