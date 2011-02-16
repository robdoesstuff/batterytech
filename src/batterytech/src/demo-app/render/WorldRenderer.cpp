/*
 * WorldRenderer.cpp
 *
 *  Created on: Sep 4, 2010
 *      Author: rgreen
 */

#include "WorldRenderer.h"
#include <batterytech/render/GraphicsConfiguration.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/platform/platformgeneral.h>
#include <batterytech/Logger.h>
#include <Math.h>
#include "../UIConstants.h"
#include "../GameUtil.h"
#include <batterytech/render/RenderContext.h>

WorldRenderer::WorldRenderer(Context *context) {
	this->context = context;
	this->gConfig = context->gConfig;
	fps = 0;
	frameSamplesCollected = 0;
	frameSampleTimeTotal = 0.0f;
	textRenderer = new TextRasterRenderer(context, UI_GAME_FONT, 12.0f);
	b2DebugRenderer = new B2DebugRenderer(context);
	ballRenderer = new BallRenderer(context);
	//uiBGRenderer = new SimpleSpriteRenderer(context, "demo_app_bg_tex.jpg");
}

WorldRenderer::~WorldRenderer() {
	logmsg("Releasing WorldRenderer");
	delete textRenderer;
	delete ballRenderer;
	//delete uiBGRenderer;
}

void WorldRenderer::init(BOOL32 newContext) {
	if (context->gConfig->useShaders) {
	} else {
		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glShadeModel(GL_SMOOTH);
	}
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	textRenderer->init(newContext);
	b2DebugRenderer->init(newContext);
	//uiBGRenderer->init(newContext);
	glViewport(0, 0, gConfig->viewportWidth, gConfig->viewportHeight);
}

void WorldRenderer::initLevel(BOOL32 newContext) {
	ballRenderer->init(newContext);
}

void WorldRenderer::render(World *world) {
	if (world->gameState == GAMESTATE_LOADING) {
		initLevel(FALSE);
		world->renderersReady = TRUE;
	}
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	context->renderContext->colorFilter = Vec4f(1, 1, 1, 1);
	if (gConfig->useShaders) {
		esMatrixLoadIdentity(&context->renderContext->projMatrix);
		esOrtho(&context->renderContext->projMatrix, WORLD_LEFT, WORLD_RIGHT, WORLD_BOTTOM, WORLD_TOP, -1, 1);
		esMatrixLoadIdentity(&context->renderContext->mvMatrix);
	} else {
		// set up world projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrthof(WORLD_LEFT, WORLD_RIGHT, WORLD_BOTTOM, WORLD_TOP, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	BOOL32 renderDebug = false;
	if (world->gameState == GAMESTATE_READY) {
		// render the main menuing bg
		//uiBGRenderer->render(WORLD_TOP, WORLD_RIGHT, WORLD_BOTTOM, WORLD_LEFT);
	} else if (world->gameState == GAMESTATE_RUNNING) {
		//uiBGRenderer->render(WORLD_TOP, WORLD_RIGHT, WORLD_BOTTOM, WORLD_LEFT);
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// pinball
		ballRenderer->render(world);
		glDisable(GL_BLEND);
		if (renderDebug) {
			// level editing and debugging
			b2DebugRenderer->render(world);
		}
		// text overlay
		frameSampleTimeTotal += context->tickDelta;
		frameSamplesCollected++;
		if (frameSamplesCollected == 10) {
			if (frameSampleTimeTotal == 0) {
				fps = 0;
			} else {
				fps = 10 / frameSampleTimeTotal;
			}
			frameSamplesCollected = 0;
			frameSampleTimeTotal = 0;
		}
		if (context->showFPS) {
			char fpsText[10];
			sprintf(fpsText, "FPS: %d", fps);
			if (gConfig->useShaders) {
				esMatrixLoadIdentity(&context->renderContext->projMatrix);
				esMatrixLoadIdentity(&context->renderContext->mvMatrix);
				esOrtho(&context->renderContext->projMatrix, 0, gConfig->width, gConfig->height, 0, -1, 1);
			} else {
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrthof(0, gConfig->width, gConfig->height, 0, -1, 1);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
			}
			textRenderer->startText();
			textRenderer->render(fpsText, 5, context->gConfig->height - 5);
			textRenderer->finishText();
		}
	}
}

void WorldRenderer::renderDebugLabels(World *world) {
}

