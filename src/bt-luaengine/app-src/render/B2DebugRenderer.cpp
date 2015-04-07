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

#ifdef BATTERYTECH_INCLUDE_BOX2D

#include "B2DebugRenderer.h"
#include <batterytech/render/Renderer.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/math/Triangulator.h>
#include <bt-box2d/Dynamics/b2Body.h>
#include <bt-box2d/Dynamics/b2World.h>
#include <bt-box2d/Dynamics/b2Fixture.h>
#include <bt-box2d/Collision/Shapes/b2Shape.h>
#include <bt-box2d/Dynamics/Joints/b2Joint.h>
#include <bt-box2d/Dynamics/Joints/b2DistanceJoint.h>
#include <math.h>
#include "../GameConstants.h"
#include "../gameobject/GameObject.h"
#include "../World.h"
#include <batterytech/render/GLResourceManager.h>
#include <batterytech/render/ShaderProgram.h>
#include <batterytech/render/RenderContext.h>

#define CIRCLE_TESSELATION 12

static ShaderProgram *shader = NULL;

B2DebugRenderer::B2DebugRenderer(GameContext *context) {
	this->context = context;
	bufferUsed = 0;
}

B2DebugRenderer::~B2DebugRenderer() {
}

void B2DebugRenderer::init(BOOL32 newGameContext) {
	ShaderProgram *shader = context->glResourceManager->getShaderProgram("line");
	if (!shader) {
		shader = new ShaderProgram("line", "shaders/lineshader.vert", "shaders/lineshader.frag");
		context->glResourceManager->addShaderProgram("line", shader);
	}
}

void B2DebugRenderer::addLine(Vector3f p1, Vector3f p2, Vector4f color) {
	if (bufferUsed + 2 >= B2DEBUG_BUFFER_LENGTH) {
		flush();
	}
	posBuffer[bufferUsed] = p1;
	posBuffer[bufferUsed+1] = p2;
	colorBuffer[bufferUsed] = color;
	colorBuffer[bufferUsed+1] = color;
	bufferUsed += 2;
}

void B2DebugRenderer::flush() {
	glFrontFace(GL_CCW);
	if (context->gConfig->useShaders) {
        Matrix4f myMv = context->renderContext->mvMatrix;
 		// mv + proj, position and color
        glUniformMatrix4fv(shader->getUniformLoc("modelview_matrix"), 1, FALSE, myMv.data);
        glUniformMatrix4fv(shader->getUniformLoc("projection_matrix"), 1, FALSE, context->renderContext->projMatrix.data);
		glVertexAttribPointer(shader->getVertexAttributeLoc("vPosition"), 3, GL_FLOAT, GL_FALSE, 0, posBuffer);
		glVertexAttribPointer(shader->getVertexAttributeLoc("vColor"), 4, GL_FLOAT, GL_FALSE, 0, colorBuffer);
        glDrawArrays(GL_LINES, 0, bufferUsed);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, posBuffer);
		glColorPointer(4, GL_FLOAT, 0, colorBuffer);
	    glDrawArrays(GL_LINES, 0, bufferUsed);
	}
	bufferUsed = 0;
}

void B2DebugRenderer::render(World *world) {
    if (!world->boxWorld) {
        return;
    }
	if (!context->gConfig->useShaders) {
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	} else {
        shader = context->glResourceManager->getShaderProgram("line");
        if (shader) {
            shader->bind();
        } else {
            // no shader
            return;
        }
    }
	world->boxWorld->SetDebugDraw(this);
	SetFlags(e_shapeBit | e_jointBit);
	world->boxWorld->DrawDebugData();
	flush();
	if (!context->gConfig->useShaders) {
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void B2DebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	for (S32 i = 0; i < vertexCount; i++) {
		S32 i2 = i+1;
		if (i2 == vertexCount) {
			i2 = 0;
		}
		addLine(Vector3f(vertices[i].x, vertices[i].y, 0), Vector3f(vertices[i2].x, vertices[i2].y, 0), Vector4f(color.r, color.g, color.b, 1));
	}
}

void B2DebugRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	for (S32 i = 0; i < vertexCount; i++) {
		S32 i2 = i+1;
		if (i2 == vertexCount) {
			i2 = 0;
		}
		addLine(Vector3f(vertices[i].x, vertices[i].y, 0), Vector3f(vertices[i2].x, vertices[i2].y, 0), Vector4f(color.r, color.g, color.b, 1));
	}
}

void B2DebugRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
	S32 vertCount = CIRCLE_TESSELATION;
	F32 radsPerSegment = TAU / CIRCLE_TESSELATION;
    Vector3f verts[CIRCLE_TESSELATION];
	for (S32 i = 0; i < vertCount; i++) {
		F32 thisX = cos(radsPerSegment * i) * radius;
		F32 thisY = sin(radsPerSegment * i) * radius;
		verts[i] = Vector3f(thisX + center.x, thisY + center.y, 0);
	}
	for (S32 i = 0; i < vertCount; i++) {
		S32 i2 = i+1;
		if (i2 == vertCount) {
			i2 = 0;
		}
		addLine(verts[i], verts[i2], Vector4f(color.r, color.g, color.b, 1));
	}
}

void B2DebugRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
	S32 vertCount = CIRCLE_TESSELATION;
	F32 radsPerSegment = TAU / CIRCLE_TESSELATION;
    Vector3f verts[CIRCLE_TESSELATION];
	for (S32 i = 0; i < vertCount; i++) {
		F32 thisX = cos(radsPerSegment * i) * radius;
		F32 thisY = sin(radsPerSegment * i) * radius;
		verts[i] = Vector3f(thisX + center.x, thisY + center.y, 0);
	}
	for (S32 i = 0; i < vertCount; i++) {
		S32 i2 = i+1;
		if (i2 == vertCount) {
			i2 = 0;
		}
		addLine(verts[i], verts[i2], Vector4f(color.r, color.g, color.b, 1));
	}
	addLine(Vector3f(center.x, center.y, 0), Vector3f(center.x+axis.x, center.y+axis.y, 0), Vector4f(color.r, color.g, color.b, 1));
}

void B2DebugRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
	addLine(Vector3f(p1.x, p1.y, 0), Vector3f(p2.x, p2.y, 0), Vector4f(color.r, color.g, color.b, 1));
}

void B2DebugRenderer::DrawTransform(const b2Transform& xf) {
	Vector3f center = Vector3f(xf.p.x, xf.p.y, 0);
	b2Vec2 offset = b2Mul(xf, b2Vec2(1,0));
	addLine(center, Vector3f(center.x + offset.x, center.y + offset.y, 0), Vector4f(1, 1, 1, 1));
}

#endif
