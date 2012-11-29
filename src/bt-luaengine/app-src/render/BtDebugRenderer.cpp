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

/*#include "BtDebugRenderer.h"
#include <batterytech/render/RenderContext.h>
#include <batterytech/Logger.h>
#include <stdio.h>

BtDebugRenderer::BtDebugRenderer(Context *context) {
	this->context = context;
	m_debugMode = 1;
	shaderProgram = new ShaderProgram("shaders/lineshader.vert", "shaders/lineshader.frag");
	lineCount = 0;
}

BtDebugRenderer::~BtDebugRenderer() {
	delete shaderProgram;
}

void BtDebugRenderer::init(BOOL32 newContext) {
	if (context->gConfig->useShaders) {
		shaderProgram->init(newContext);
		shaderProgram->addVertexAttributeLoc("vPosition");
		shaderProgram->addVertexAttributeLoc("vColor");
		shaderProgram->addUniformLoc("projection_matrix");
		shaderProgram->addUniformLoc("modelview_matrix");
	}
	checkGLError("BtDebugRenderer init");
}

void BtDebugRenderer::start() {
	checkGLError("BtDebugRenderer Start");
	glFrontFace(GL_CW);
	if (context->gConfig->useShaders) {
		shaderProgram->bind();
		//glEnable(0x8642);
	}
	lineCount = 0;
}

void BtDebugRenderer::end() {
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
	checkGLError("BtDebugRenderer end");
}

void BtDebugRenderer::drawLine(const btVector3& from, const btVector3& to,
		const btVector3& color) {
	if (lineCount < BT_DEBUG_MAX_LINES) {
		vPos[lineCount * 6] = from.getX();
		vPos[lineCount * 6 + 1] = from.getY();
		vPos[lineCount * 6 + 2] = from.getZ();
		vPos[lineCount * 6 + 3] = to.getX();
		vPos[lineCount * 6 + 4] = to.getY();
		vPos[lineCount * 6 + 5] = to.getZ();
		vColor[lineCount * 8] = color.getX();
		vColor[lineCount * 8 + 1] = color.getY();
		vColor[lineCount * 8 + 2] = color.getZ();
		vColor[lineCount * 8 + 3] = 1.0;
		vColor[lineCount * 8 + 4] = color.getX();
		vColor[lineCount * 8 + 5] = color.getY();
		vColor[lineCount * 8 + 6] = color.getZ();
		vColor[lineCount * 8 + 7] = 1.0;
		lineCount++;
	}
}

void BtDebugRenderer::setDebugMode(int debugMode) {
	m_debugMode = debugMode;
}

void BtDebugRenderer::draw3dText(const btVector3& location,
		const char* textString) {
	//glRasterPos3f(location.x(),  location.y(),  location.z());
	//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),textString);
}

void BtDebugRenderer::reportErrorWarning(const char* warningString) {
	printf(warningString);
}

void BtDebugRenderer::drawContactPoint(const btVector3& pointOnB,
		const btVector3& normalOnB, btScalar distance, int lifeTime,
		const btVector3& color) {
	{
		//btVector3 to=pointOnB+normalOnB*distance;
		//const btVector3&from = pointOnB;
		//glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);

		//GLDebugDrawer::drawLine(from, to, color);

		//glRasterPos3f(from.x(),  from.y(),  from.z());
		//char buf[12];
		//sprintf(buf," %d",lifeTime);
		//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);
	}
}*/
