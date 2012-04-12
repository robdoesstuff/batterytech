/*
 * HelloWorldApp.h
 *
 *  Created on: Oct 10, 2010
 *      Author: rgreen
 */

#ifndef HELLOWORLDAPP_H_
#define HELLOWORLDAPP_H_

#include <batterytech/primitives.h>
#include <batterytech/batterytech_entrypoints.h>

namespace BatteryTech { class Context; }

using namespace BatteryTech;

class HelloWorldApp : public BTApplicationUpdater, public BTApplicationRenderer {
public:
	HelloWorldApp(Context *context);
	virtual ~HelloWorldApp();
	virtual void update();
	virtual void render();
private:
	Context *context;
	BOOL32 initialized;
};

#endif /* HELLOWORLDAPP_H_ */
