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
#include <math.h>
#include "../UIConstants.h"
#include "../GameUtil.h"
#include "../GameContext.h"
#include <batterytech/render/RenderContext.h>
#include <batterytech/render/MenuRenderer.h>
#include <batterytech/render/GLResourceManager.h>
#include <batterytech/render/QuadRenderer.h>

WorldRenderer::WorldRenderer(GameContext *context) {
	this->context = context;
	this->gConfig = context->gConfig;
	fps = 0;
	frameSamplesCollected = 0;
	frameSampleTimeTotal = 0.0f;
	textRenderer = new TextRasterRenderer(context, UI_GAME_FONT, 32.0f);
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
	if (newContext) {
		context->glResourceManager->invalidateGL();
	}
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
	context->quadRenderer->init(TRUE);
	context->glResourceManager->loadTextures();
	context->glResourceManager->loadShaderPrograms();
	textRenderer->init(newContext);
	b2DebugRenderer->init(newContext);
	ballRenderer->init(newContext);
	//uiBGRenderer->init(newGameContext);
	glViewport(0, 0, gConfig->viewportWidth, gConfig->viewportHeight);
}

void WorldRenderer::render() {
	World *world = context->world;
	if (world->gameState == GAMESTATE_LOADING) {
		world->renderersReady = TRUE;
	}
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	context->renderContext->colorFilter = Vector4f(1, 1, 1, 1);
	if (gConfig->useShaders) {
		context->renderContext->projMatrix.identity();
		context->renderContext->projMatrix.ortho(WORLD_LEFT, WORLD_RIGHT, WORLD_BOTTOM, WORLD_TOP, -1, 1);
		context->renderContext->mvMatrix.identity();
	} else {
		// set up world projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrthof(WORLD_LEFT, WORLD_RIGHT, WORLD_BOTTOM, WORLD_TOP, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_2D);
	}
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
		// BT UI
		context->menuRenderer->render();
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
				context->renderContext->projMatrix.identity();
				context->renderContext->mvMatrix.identity();
				context->renderContext->projMatrix.ortho(0, gConfig->width, gConfig->height, 0, -1, 1);
			} else {
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrthof(0, gConfig->width, gConfig->height, 0, -1, 1);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
			}
			glEnable(GL_BLEND);
			textRenderer->startText();
			textRenderer->render(fpsText, 5, context->gConfig->height - 5);
			textRenderer->finishText();
		}
	}
}

void WorldRenderer::renderDebugLabels(World *world) {
}

