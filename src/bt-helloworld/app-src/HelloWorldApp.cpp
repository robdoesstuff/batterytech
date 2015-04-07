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
#include <batterytech/render/MenuRenderer.h>
#include <string.h>
#include "RenderUtil.h"
#include <batterytech/render/QuadRenderer.h>
#include <batterytech/render/GLResourceManager.h>

Context* btAppCreateContext(GraphicsConfiguration *graphicsConfig) {
	return new AppContext(graphicsConfig);
}

HelloWorldApp::HelloWorldApp(AppContext *context) {
	this->context = context;
	initialized = FALSE;
	if (!context->appProperties) {
		return;
	}
	textRenderer = new TextRasterRenderer(context, context->appProperties->get("menu_font")->getValue(), 
                                          context->appProperties->get("ui_font_size")->getFloatValue());
}

HelloWorldApp::~HelloWorldApp() {
	logmsg("Releasing App");
	initialized = FALSE;
	context = NULL;
	delete textRenderer;
	textRenderer = NULL;
	// context is freed by batterytech core
}

void HelloWorldApp::update() {
	if (!initialized || context->newGraphicsContext) {
		logmsg("Initializing Renderers");
		RenderUtil::setupGL(context);
		context->quadRenderer->init(TRUE);
		context->glResourceManager->loadTextures();
		context->glResourceManager->loadShaderPrograms();
		textRenderer->init(TRUE);
		context->menuRenderer->init(TRUE);
		context->newGraphicsContext = FALSE;
		initialized = TRUE;
	}
}

void HelloWorldApp::render() {
	if (initialized) {
 		RenderUtil::startGLRender(context);
		textRenderer->startText();
		F32 width = textRenderer->measureWidth("Hello World!");
		textRenderer->render("Hello World!", context->gConfig->width/2 - width/2, context->gConfig->height/2 + textRenderer->getHeight(1.0f)/2, 1.0f);
		textRenderer->finishText();
	}
}
