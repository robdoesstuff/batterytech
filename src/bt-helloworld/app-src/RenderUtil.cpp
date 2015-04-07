
#include "RenderUtil.h"
#include <batterytech/platform/platformgl.h>
#include <batterytech/render/RenderContext.h>
#include <batterytech/render/GraphicsConfiguration.h>
#include "AppContext.h"

void RenderUtil::setupGL(AppContext *context) {
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
    glFrontFace(GL_CW);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, context->gConfig->viewportWidth, context->gConfig->viewportHeight);
}

void RenderUtil::startGLRender(AppContext *context) {
	glViewport(0, 0, context->gConfig->viewportWidth, context->gConfig->viewportHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
}
