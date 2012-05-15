/*
 * SimpleSpriteRenderer.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: rgreen
 */

#include "SimpleSpriteRenderer.h"
#include "../GameConstants.h"
#include <stdio.h>
#include <string.h>
#include <batterytech/Logger.h>
#include <batterytech/util/esTransform.h>
#include <batterytech/render/GraphicsConfiguration.h>
#include <batterytech/render/RenderContext.h>
#include <batterytech/render/GLResourceManager.h>
#include "../World.h"
#include "../GameContext.h"

SimpleSpriteRenderer::SimpleSpriteRenderer(GameContext *context) {
	this->context = context;
	shaderProgram = new ShaderProgram("shaders/quadshader.vert", "shaders/quadshader.frag");
}

SimpleSpriteRenderer::~SimpleSpriteRenderer() {
	delete shaderProgram;
}

void SimpleSpriteRenderer::init(BOOL32 newContext) {
	if (context->gConfig->useShaders) {
		shaderProgram->init(newContext);
		shaderProgram->addVertexAttributeLoc("vPosition");
		shaderProgram->addVertexAttributeLoc("uvMap");
		shaderProgram->addUniformLoc("projection_matrix");
		shaderProgram->addUniformLoc("modelview_matrix");
		shaderProgram->addUniformLoc("tex");
		shaderProgram->addUniformLoc("colorFilter");
	}
	checkGLError("SimpleSpriteRenderer Init");
}

void SimpleSpriteRenderer::render(F32 top, F32 right, F32 bottom, F32 left) {
	F32 verts[] = {
			left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
	};
	F32 uvs[] = {
			0, 0, 1, 0, 1, 1, 0, 1
	};
	//glFrontFace(GL_CW);
	if (context->gConfig->useShaders) {
		shaderProgram->bind();
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, 0, uvs);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->mvMatrix.data);
		glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
		Vector4f colorFilter = context->renderContext->colorFilter;
		glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		shaderProgram->unbind();
	} else {
		glVertexPointer(3, GL_FLOAT, 0, &verts);
		glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
}

void SimpleSpriteRenderer::render(F32 x, F32 y, F32 width, F32 height, F32 angleRads) {
	// this one
	//glFrontFace(GL_CW);
	F32 top = height/2;
	F32 right = width/2;
	F32 bottom = -height/2;
	F32 left = -width/2;
	F32 verts[] = {
			left, top, 0, right, top, 0, right, bottom, 0, left, bottom, 0
	};
	F32 uvs[] = {
			0, 0, 1, 0, 1, 1, 0, 1
	};
	if (context->gConfig->useShaders) {
		Matrix4f myMvMatrix = context->renderContext->mvMatrix;
		myMvMatrix.translate(x, y, 0);
		myMvMatrix.rotate(angleRads * (180 / PI), 0, 0, -1.0f);
		shaderProgram->bind();
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, 0, uvs);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) myMvMatrix.data);
		glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
		Vector4f colorFilter = context->renderContext->colorFilter;
		glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		shaderProgram->unbind();
	} else {
		// GL1 rendering branch
		Vector4f colorFilter = context->renderContext->colorFilter;
		glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		glPushMatrix();
		glTranslatef(x, y, 0);
		glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
		glVertexPointer(3, GL_FLOAT, 0, &verts);
		glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
	}

}

void SimpleSpriteRenderer::render(RenderItem *item) {
	//glFrontFace(GL_CW);
	F32 x = item->pos.x;
	F32 y = item->pos.y;
	F32 z = item->pos.z;
	F32 width = item->scale.x;
	F32 height = item->scale.y;
	F32 angleRads = item->orientation.v.z;
	F32 top;
	F32 bottom;
	F32 right = width/2;
	if (item->renderType == RenderItem::RENDERTYPE_BB) {
		top = height/2;
		bottom = -height/2;
	} else {
		top = -height/2;
		bottom = height/2;
	}
	F32 left = -width/2;
	Vector4f myUvs = item->uvs;
	F32 verts[] = {
			left, top, z, right, top, z, right, bottom, z, left, bottom, z
	};
	F32 uvs[] = {
			myUvs.x, myUvs.y, myUvs.z, myUvs.y, myUvs.z, myUvs.w, myUvs.x, myUvs.w
	};
	if (item->textureName[0]) {
		Texture *texture = context->glResourceManager->getTexture(item->textureName);
		if (texture) {
			texture->bind();
		}
	}
	F32 olda = context->renderContext->colorFilter.a;
	if(!(item->flags & RENDERITEM_FLAG_IS_OPAQUE) && item->alpha < 1) {
		context->renderContext->colorFilter.a = item->alpha;
	}
	if (context->gConfig->useShaders) {
		Matrix4f myMvMatrix = context->renderContext->mvMatrix;
		myMvMatrix.translate(x, y, z);
		myMvMatrix.rotate(angleRads * (180 / PI), 0, 0, -1.0f);
		if (item->renderType == RenderItem::RENDERTYPE_BB) {
			Vector3f dir = context->world->camera->pos - item->pos;
			dir.normalize();
			Vector3f newY = context->world->camera->invRotMatrix * Vector3f(0,1,0);
			Vector3f newX = context->world->camera->invRotMatrix * Vector3f(1,0,0);
			//F32 angle = atan2f(dir.y, dir.x);
			//myMvMatrix.rotate(angle, 0, 0, 1.0f);
			Matrix4f mat = Matrix4f();
			mat.data[0] = newX.x;
			mat.data[1] = newX.y;
			mat.data[2] = newX.z;
			mat.data[4] = newY.x;
			mat.data[5] = newY.y;
			mat.data[6] = newY.z;
			mat.data[8] = dir.x;
			mat.data[9] = dir.y;
			mat.data[10] = dir.z;
			myMvMatrix = myMvMatrix * mat;
		}
		shaderProgram->bind();
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(shaderProgram->getVertexAttributeLoc("uvMap"), 2, GL_FLOAT, GL_FALSE, 0, uvs);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("projection_matrix"), 1, GL_FALSE, (GLfloat*) context->renderContext->projMatrix.data);
		glUniformMatrix4fv(shaderProgram->getUniformLoc("modelview_matrix"), 1, GL_FALSE, (GLfloat*) myMvMatrix.data);
		glUniform1i(shaderProgram->getUniformLoc("tex"), 0);
		Vector4f colorFilter = context->renderContext->colorFilter;
		glUniform4f(shaderProgram->getUniformLoc("colorFilter"), colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		shaderProgram->unbind();
	} else {
		// GL1 rendering branch
		Vector4f colorFilter = context->renderContext->colorFilter;
		glColor4f(colorFilter.x,colorFilter.y,colorFilter.z,colorFilter.a);
		glPushMatrix();
		glTranslatef(x, y, 0);
		glRotatef(angleRads * (180 / PI), 0, 0, 1.0f);
		glVertexPointer(3, GL_FLOAT, 0, &verts);
		glTexCoordPointer(2, GL_FLOAT, 0, &uvs);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
	}
	context->renderContext->colorFilter.a = olda;
}

