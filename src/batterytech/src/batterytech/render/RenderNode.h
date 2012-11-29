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

#ifndef RENDERNODE_H_
#define RENDERNODE_H_

#include <batterytech/math/Matrix4.h>
#include <batterytech/util/ManagedArray.h>
#include "../math/Vector3.h"
#include <batterytech/math/AABB3.h>

namespace BatteryTech {

    /** Each mesh can have a different bone structure, sharing bones with other meshes in the same tree */
    struct RenderNodeMeshBone {
        BOOL32 isUsed;
        /** The bone's local offset matrix */
        Matrix4f boneOffset;
        /** Which mesh this bone is for */
        U32 boneMeshIdx;
        /** The matrix index in the flattened mesh bone matrix table */
        S32 matrixIdx;
    };
    
	class RenderNode {
	public:
		RenderNode();
		virtual ~RenderNode();
		RenderNode *parentNode;
		ManagedArray<RenderNode> *childNodes;
		Matrix4f localTransform;
		Matrix4f globalTransform;
		BOOL32 isBone;
        RenderNodeMeshBone* meshBones;
 		char *name;
		U32 meshCount;
		/** The meshes of this node. Each entry is an index into the mesh */
		U32* meshIndices;
		/** use these points to determine a broad cull */
		AABB3f localAABB, globalAABB;
	};

}

#endif /* RENDERNODE_H_ */
