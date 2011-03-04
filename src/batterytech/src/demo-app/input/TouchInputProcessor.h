/*
 * TouchInputProcessor.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rgreen
 */

#ifndef TOUCHINPUTPROCESSOR_H_
#define TOUCHINPUTPROCESSOR_H_

#include <batterytech/Context.h>
#include <batterytech/primitives.h>

using namespace BatteryTech;

class GameObject;

class TouchInputProcessor {
public:
	TouchInputProcessor();
	virtual ~TouchInputProcessor();
	void processInput(Context *context);
private:
	BOOL32 *lastTouchDown;
	GameObject **touchedObj;
};

#endif /* TOUCHINPUTPROCESSOR_H_ */
