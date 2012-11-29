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
// Name        : GLModelBinding.h
// Description : A helper class to assist with VBO-VertexAttrib bindings
//============================================================================

#ifndef GLMODELBINDING_H_
#define GLMODELBINDING_H_

#include "../platform/platformgl.h"

namespace BatteryTech {

class GLModelBinding {
public:
	GLModelBinding();
	virtual ~GLModelBinding();
	void init(F32 *positions, F32 *normals, F32 *tex, S32 vCount);
	void clearGL();
	void bindPositionsToShader(GLint attributeLoc);
	void bindNormalsToShader(GLint attributeLoc);
	void bindUVsToShader(GLint attributeLoc);
	void unbind();
	GLuint vPosVBOId;
	GLuint vNormalVBOId;
	GLuint vTexVBOId;
	S32 vCount;
	void *model;
	char *name;
};

}

#endif /* GLMODELBINDING_H_ */
