/*
 * HelloWorldApp.cpp
 *
 *  Created on: Oct 10, 2010
 *      Author: rgreen
 */

#include "HelloWorldApp.h"
#include "AppContext.h"
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/Logger.h>
#include <string.h>

Context* btAppCreateContext(GraphicsConfiguration *graphicsConfig) {
	return new AppContext(graphicsConfig);
}

HelloWorldApp::HelloWorldApp(Context *context) {
	this->context = context;
}

HelloWorldApp::~HelloWorldApp() {
	logmsg("Releasing App");
	initialized = FALSE;
	context = NULL;
	// context is freed by batterytech core
}

void HelloWorldApp::update() {
	if (!initialized || context->wasSuspended) {
		logmsg("Initializing Renderers");
		//context->worldRenderer->init(TRUE);
		context->wasSuspended = FALSE;
		initialized = TRUE;
	}
}

void HelloWorldApp::render() {

}
