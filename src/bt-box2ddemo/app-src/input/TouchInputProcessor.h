/*
 * TouchInputProcessor.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rgreen
 */

#ifndef TOUCHINPUTPROCESSOR_H_
#define TOUCHINPUTPROCESSOR_H_

#include "../GameContext.h"
#include <batterytech/primitives.h>

using namespace BatteryTech;

class GameObject;

class TouchInputProcessor {
public:
	TouchInputProcessor();
	virtual ~TouchInputProcessor();
	void processInput(GameContext *context);
private:
	BOOL32 *lastTouchDown;
	GameObject **touchedObj;
};

#endif /* TOUCHINPUTPROCESSOR_H_ */
