/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

/*#include "IndicatorRenderer.h"
#include "../gameobject/GameObject.h"
#include <batterytech/render/RenderContext.h>
#include <batterytech/render/Texture.h>
#include <batterytech/render/GLResourceManager.h>
#include <batterytech/Logger.h>
#include "../World.h"
#include "../gameobject/GameObjectLuaBinder.h"
#include "../GameUtil.h"
#include "SimpleSpriteRenderer.h"

IndicatorRenderer::IndicatorRenderer(Context *context) {
	this->context = context;
	shaderProgram = new ShaderProgram("shaders/lineshader.vert", "shaders/lineshader.frag");
	spriteRenderer = new SimpleSpriteRenderer(context);
	lineCount = 0;
}

IndicatorRenderer::~IndicatorRenderer() {
	delete shaderProgram;
	delete spriteRenderer;
}

void IndicatorRenderer::init(BOOL32 newContext) {
	spriteRenderer->init(newContext);
	if (context->gConfig->useShaders) {
		shaderProgram->init(newContext);
		shaderProgram->addVertexAttributeLoc("vPosition");
		shaderProgram->addVertexAttributeLoc("vColor");
		shaderProgram->addUniformLoc("projection_matrix");
		shaderProgram->addUniformLoc("modelview_matrix");
	}
	checkGLError("IndicatorRenderer init");
}

void IndicatorRenderer::render(World *world) {
	checkGLError("IndicatorRenderer Start");
	glFrontFace(GL_CW);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);
	if (context->gConfig->useShaders) {
		shaderProgram->bind();
		//glEnable(0x8642);
	}
	lineCount = 0;

	//drawLine(world->levelClick, Vector3f(world->levelClick.x, world->levelClick.y, world->levelClick.z + 20), Vector3f(1,0,0));
	for (S32 i = 0; i < world->gameObjects->getSize(); i++) {
		btVector3 aabbMin, aabbMax;
		F32 height, width;
		Vector3f zLift(0,0,0.1f);
		GameObject *obj = world->gameObjects->array[i];
		if (!obj->luaBinder->getBool("showIndicators")) {
			continue;
		}
		if (obj->isSelected) {
			if (obj->btBody) {
				obj->btBody->getAabb(aabbMin, aabbMax);
			} else if (obj->btGhost) {
				obj->btGhost->getCollisionShape()->getAabb(obj->btGhost->getWorldTransform(),aabbMin,aabbMax);
			}
			height = aabbMax.y() - aabbMin.y() + 0.15f;
			width = aabbMax.x() - aabbMin.x() + 0.15f;
			F32 x = obj->pos.x;
			F32 y = obj->pos.y;
			F32 z = obj->pos.z - (aabbMax.z() - aabbMin.z()) / 2 + 0.1f;
			F32 hHeight = height / 2;
			F32 hWidth = width / 2;
			drawLine(Vector3f(x - hWidth, y - hHeight, z), Vector3f(x - hWidth, y + hHeight, z), Vector3f(1,0,0));
			drawLine(Vector3f(x - hWidth, y + hHeight, z), Vector3f(x + hWidth, y + hHeight, z), Vector3f(1,0,0));
			drawLine(Vector3f(x + hWidth, y + hHeight, z), Vector3f(x + hWidth, y - hHeight, z), Vector3f(1,0,0));
			drawLine(Vector3f(x + hWidth, y - hHeight, z), Vector3f(x - hWidth, y - hHeight, z), Vector3f(1,0,0));
		}
		if (obj->pathLength > 0 && obj->nextPathSeek != -1) {
			drawLine(obj->pos, obj->path[obj->nextPathSeek].pos, Vector3f(1,0,0));
			for (S32 j = obj->nextPathSeek; j < obj->pathLength - 1; j++) {
				drawLine(obj->path[j].pos + zLift, obj->path[j+1].pos + zLift, Vector3f(1,0,0));
			}
		}
	}
	// draw line batch
	if (context->gConfig->useShaders) {
		//glUniform4f(shaderColorFilter, color.getX(), color.getY(), color.getZ(), 1.0f);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, vPos);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vColor"), 4, GL_FLOAT, GL_FALSE, 0, vColor);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, context->renderContext->projMatrix);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, context->renderContext->mvMatrix);
		glDrawArrays(GL_POINTS, 0, lineCount * 2);
		glDrawArrays(GL_LINES, 0, lineCount * 2);
	} else {
		//glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);
		glVertexPointer(3, GL_FLOAT, 0, &vPos);
		glColorPointer(4, GL_FLOAT, 0, &vColor);
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, lineCount);
		glDrawArrays(GL_LINES, 0, lineCount);
	}
	if (context->gConfig->useShaders) {
		shaderProgram->unbind();
		//glDisable(0x8642);
		glUseProgram(0);
	}
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	checkGLError("IndicatorRenderer end");
}

void IndicatorRenderer::render2D() {
	checkGLError("IndicatorRenderer 2D Start");
	// TODO - draw 2D indicators
	// for each gameobject
	// get indicator texture - if set, draw.
	ManagedArray<GameObject> *gameObjects = context->world->gameObjects;
	F32 height = 30 * context->gConfig->uiScale;
	F32 width = 30 * context->gConfig->uiScale;
	F32 vOffset = -10 * context->gConfig->uiScale;
	for (S32 i = 0; i < gameObjects->getSize(); i++) {
		GameObject *obj = gameObjects->array[i];
		// TODO - don't use allocating string call - add reusable buffer call to lua binder
		char *textureName = obj->luaBinder->getString("indicatorTexture");
		if (textureName) {
			Texture *t = context->glResourceManager->getTexture(textureName);
			delete [] textureName;
			if (t) {
				t->bind();
			}
			const Vector3f screenCoord = GameUtil::getScreenCoord(obj->pos, context);
			spriteRenderer->render(screenCoord.y - vOffset - height, screenCoord.x + width / 2, screenCoord.y - vOffset, screenCoord.x - width / 2);
		}
	}
}

void IndicatorRenderer::drawLine(const Vector3f& from, const Vector3f& to,
		const Vector3f& color) {
	if (lineCount < INDICATOR_MAX_LINES) {
		vPos[lineCount * 6] = from.x;
		vPos[lineCount * 6 + 1] = from.y;
		vPos[lineCount * 6 + 2] = from.z;
		vPos[lineCount * 6 + 3] = to.x;
		vPos[lineCount * 6 + 4] = to.y;;
		vPos[lineCount * 6 + 5] = to.z;
		vColor[lineCount * 8] = color.x;
		vColor[lineCount * 8 + 1] = color.y;
		vColor[lineCount * 8 + 2] = color.z;
		vColor[lineCount * 8 + 3] = 1.0;
		vColor[lineCount * 8 + 4] = color.x;
		vColor[lineCount * 8 + 5] = color.y;
		vColor[lineCount * 8 + 6] = color.z;
		vColor[lineCount * 8 + 7] = 1.0;
		lineCount++;
	}
}
*/
