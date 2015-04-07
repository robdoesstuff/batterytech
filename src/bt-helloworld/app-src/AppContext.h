/*
 * AppContext.h
 *
 *  Created on: Apr 9, 2012
 *      Author: rgreen
 */

#ifndef APPCONTEXT_H_
#define APPCONTEXT_H_

#include <batterytech/Context.h>

using namespace BatteryTech;

class HelloWorldApp;

class AppContext : public Context {
public:
	AppContext(GraphicsConfiguration *gConfig);
	virtual ~AppContext();
	HelloWorldApp *app;
};

#endif /* GAMECONTEXT_H_ */
