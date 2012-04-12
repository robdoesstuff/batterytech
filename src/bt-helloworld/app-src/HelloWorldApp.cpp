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
#include <batterytech/platform/platformgl.h>

#include <string.h>

Context* btAppCreateContext(GraphicsConfiguration *graphicsConfig) {
	return new AppContext(graphicsConfig);
}

HelloWorldApp::HelloWorldApp(Context *context) {
	this->context = context;
	textRenderer = new TextRasterRenderer(context, context->appProperties->get("menu_font")->getValue(), 24.0f);
}

HelloWorldApp::~HelloWorldApp() {
	logmsg("Releasing App");
	initialized = FALSE;
	context = NULL;
	delete textRenderer;
	textRenderer = NULL;
	// context is freed by batterytech core
}

void HelloWorldApp::setupGL() {
	if (context->gConfig->useShaders) {
	} else {
		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_TEXTURE_2D);
	}
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, context->gConfig->viewportWidth, context->gConfig->viewportHeight);
}

void HelloWorldApp::update() {
	if (!initialized || context->wasSuspended) {
		logmsg("Initializing Renderers");
		//context->worldRenderer->init(TRUE);
		setupGL();
		textRenderer->init(TRUE);
		context->wasSuspended = FALSE;
		initialized = TRUE;
	}
}

void HelloWorldApp::render() {
	if (initialized) {
		glClear(GL_COLOR_BUFFER_BIT);
		textRenderer->startText();
		textRenderer->render("Hello World!", 300, 300, 1.0f);
		textRenderer->finishText();
	}
}
