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
#include <batterytech/render/MenuRenderer.h>
#include <batterytech/render/RenderContext.h>
#include <string.h>

Context* btAppCreateContext(GraphicsConfiguration *graphicsConfig) {
	return new AppContext(graphicsConfig);
}

HelloWorldApp::HelloWorldApp(Context *context) {
	this->context = context;
	initialized = FALSE;
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
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, context->gConfig->viewportWidth, context->gConfig->viewportHeight);
}

void HelloWorldApp::update() {
	if (!initialized || context->wasSuspended) {
		logmsg("Initializing Renderers");
		setupGL();
		textRenderer->init(TRUE);
		context->menuRenderer->init(TRUE);
		context->wasSuspended = FALSE;
		initialized = TRUE;
	}
}

void HelloWorldApp::render() {
	if (initialized) {
		glClear(GL_COLOR_BUFFER_BIT);
		context->renderContext->colorFilter = Vector4f(1, 1, 1, 1);
		if (context->gConfig->useShaders) {
			context->renderContext->projMatrix.identity();
			context->renderContext->mvMatrix.identity();
			context->renderContext->projMatrix.ortho(0, context->gConfig->width, context->gConfig->height, 0, -1, 1);
		} else {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrthof(0, context->gConfig->width, context->gConfig->height, 0, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}
		textRenderer->startText();
		F32 width = textRenderer->measureWidth("Hello World!");
		textRenderer->render("Hello World!", context->gConfig->width/2 - width/2, context->gConfig->height/2 + textRenderer->getHeight(1.0f)/2, 1.0f);
		textRenderer->finishText();
	}
}
