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

//============================================================================
// Name        : GLModelBinding.cpp
// Description : A helper class to assist with VBO-VertexAttrib bindings
//============================================================================

#include "GLModelBinding.h"
#include "../platform/platformgl.h"

namespace BatteryTech {

GLModelBinding::GLModelBinding() {
	vPosVBOId = vNormalVBOId = vTexVBOId = 0;
	vCount = 0;
	model = NULL;
	name = NULL;
}

GLModelBinding::~GLModelBinding() {
	model = NULL;
	if (name) {
		delete [] name;
	}
	name = NULL;
}

void GLModelBinding::init(F32 *positions, F32 *normals, F32 *tex, S32 vCount) {
	this->vCount = vCount;
	GLuint bufferIds[3];
	glGenBuffers(3, bufferIds);
	vPosVBOId = bufferIds[0];
	vNormalVBOId = bufferIds[1];
	vTexVBOId = bufferIds[2];
	glBindBuffer(GL_ARRAY_BUFFER, vPosVBOId);
	glBufferData(GL_ARRAY_BUFFER, vCount * 3 * 4, positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vNormalVBOId);
	glBufferData(GL_ARRAY_BUFFER, vCount * 3 * 4, normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vTexVBOId);
	glBufferData(GL_ARRAY_BUFFER, vCount * 2 * 4, tex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLModelBinding::clearGL() {
	if (vPosVBOId) {
		glDeleteBuffers(1, &vPosVBOId);
	}
	vPosVBOId = 0;
	if (vNormalVBOId) {
		glDeleteBuffers(1, &vNormalVBOId);
	}
	vNormalVBOId = 0;
	if (vTexVBOId) {
		glDeleteBuffers(1, &vTexVBOId);
	}
	vTexVBOId = 0;
}

void GLModelBinding::bindPositionsToShader(GLint attributeLoc) {
	glBindBuffer(GL_ARRAY_BUFFER, vPosVBOId);
	glVertexAttribPointer(attributeLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void GLModelBinding::bindNormalsToShader(GLint attributeLoc) {
	glBindBuffer(GL_ARRAY_BUFFER, vNormalVBOId);
	glVertexAttribPointer(attributeLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void GLModelBinding::bindUVsToShader(GLint attributeLoc) {
	glBindBuffer(GL_ARRAY_BUFFER, vTexVBOId);
	glVertexAttribPointer(attributeLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void GLModelBinding::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}
