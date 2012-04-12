/*
 * AppContext.cpp
 *
 *  Created on: Apr 9, 2012
 *      Author: rgreen
 */

#include "AppContext.h"
#include "HelloWorldApp.h"

AppContext::AppContext(GraphicsConfiguration *gConfig) : Context(gConfig) {
	app = new HelloWorldApp(this);
	appUpdater = app;
	appRenderer = app;
}

AppContext::~AppContext() {
	delete app;
	app = NULL;
}
