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
// Name        : RenderNode.cpp
// Description : A basic renderable node
//============================================================================

#include "RenderNode.h"

namespace BatteryTech {

	RenderNode::RenderNode() {
		parentNode = NULL;
		childNodes = NULL;
        meshBones = NULL;
		localTransform = Matrix4f();
		globalTransform = Matrix4f();
		name = NULL;
		meshCount = 0;
		meshIndices = NULL;
		isBone = FALSE;
		localAABB = AABB3f();
		globalAABB = AABB3f();
	}

	RenderNode::~RenderNode() {
        delete [] meshIndices;
		delete [] name;
        delete [] meshBones;
	}

}
