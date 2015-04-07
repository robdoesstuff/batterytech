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
#include <batterytech/render/TextRasterRenderer.h>


using namespace BatteryTech;

class AppContext;

class HelloWorldApp : public BTApplicationUpdater, public BTApplicationRenderer {
public:
	HelloWorldApp(AppContext *context);
	virtual ~HelloWorldApp();
	virtual void update();
	virtual void render();
private:
	void setupGL();
	AppContext *context;
	BOOL32 initialized;
	TextRasterRenderer *textRenderer;
};

#endif /* HELLOWORLDAPP_H_ */
