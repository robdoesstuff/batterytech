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

WorldRenderer::WorldRenderer(Context *context) {
	this->context = context;
	this->gConfig = context->gConfig;
	fps = 0;
	frameSamplesCollected = 0;
	frameSampleTimeTotal = 0.0f;
	textRenderer = new TextRasterRenderer(gConfig, UI_GAME_FONT, 12.0f);
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
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
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
	glColor4f(1, 1, 1, 1);
	// set up world projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(WORLD_LEFT, WORLD_RIGHT, WORLD_BOTTOM, WORLD_TOP, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_BLEND);
	BOOL32 renderDebug = false;
	if (world->gameState == GAMESTATE_READY) {
		// render the main menuing bg
		//uiBGRenderer->render(WORLD_TOP, WORLD_RIGHT, WORLD_BOTTOM, WORLD_LEFT);
	} else if (world->gameState == GAMESTATE_RUNNING) {
		// multitouch debugging
		/*
		if (context->pointerState[0].isDown) {
			F32 verts[] = { WORLD_LEFT, WORLD_TOP, 0,
					WORLD_LEFT, WORLD_BOTTOM, 0,
					WORLD_RIGHT/2, WORLD_BOTTOM, 0,
					WORLD_RIGHT/2, WORLD_TOP, 0 };
			glDisable(GL_TEXTURE_2D);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glFrontFace(GL_CCW);
			glVertexPointer(3, GL_FLOAT, 0, &verts);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnable(GL_TEXTURE_2D);
		}
		if (context->pointerState[1].isDown) {
			F32 verts[] = { WORLD_RIGHT/2, WORLD_TOP, 0,
					WORLD_RIGHT/2, WORLD_BOTTOM, 0,
					WORLD_RIGHT, WORLD_BOTTOM, 0,
					WORLD_RIGHT, WORLD_TOP, 0 };
			glDisable(GL_TEXTURE_2D);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glFrontFace(GL_CCW);
			glVertexPointer(3, GL_FLOAT, 0, &verts);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnable(GL_TEXTURE_2D);
		}
		*/
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
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrthof(0, gConfig->width, gConfig->height, 0, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			textRenderer->startText();
			textRenderer->render(fpsText, 5, context->gConfig->height - 5);
			textRenderer->finishText();
		}
	}
}

void WorldRenderer::renderDebugLabels(World *world) {
}

