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
// Name        : AssimpAnimator.h
// Description : Open Asset Importer Scene Animator
//============================================================================

#ifndef ASSIMPANIMATOR_H_
#define ASSIMPANIMATOR_H_

#include <batterytech/math/Vector4.h>
#include <batterytech/util/HashTable.h>
#include <batterytech/math/Matrix4.h>
#include "../batterytech_globals.h"
#include "RenderNode.h"
#include "GLAssimpMeshBinding.h"
#include "GLAssimpBinding.h"

#ifdef BATTERYTECH_INCLUDE_ASSIMP

#include "../../bt-assimp/include/aiScene.h"

namespace BatteryTech {

struct MeshBoneMatrices {
    MeshBoneMatrices() {
        boneMatrices = NULL;
        boneMatricesWithRootInv = NULL;
        boneMatrixCount = 0;
    }
    virtual ~MeshBoneMatrices() {
        delete [] boneMatrices;
        delete [] boneMatricesWithRootInv;
        delete boneMatrixIdxTable;        
    }
	// Ordered list of global bone transforms, 1 per bone.
	Matrix4f *boneMatrices;
	S32 boneMatrixCount;
	// Ordered list of global bone transforms multiplied with a scene root inverse, 1 per bone.
	Matrix4f *boneMatricesWithRootInv;
	// BoneName->boneMatrices idx - eg: Matrix4f m = boneMatrices[boneMatrixIdxTable->get("HipJoint")]
	StrHashTable <S32> *boneMatrixIdxTable;
};
    
class AssimpAnimator {
public:
	AssimpAnimator();
	virtual ~AssimpAnimator();
	void init(GLAssimpBinding *assimpBinding, const char *meshName);
	void interpolate(F32 time);
	void applyTransforms(RenderNode *meshNode, const aiMesh *mesh, GLAssimpSkinnedMeshVertex *vertAtts);
	Matrix4f* updateGlobalInverseBoneMatrices(RenderNode *meshNode, const aiMesh *mesh);
	RenderNode *rootNode;
	S32 nodeCount;
	// BoneName->RenderNode
	StrHashTable <RenderNode*> *nodeTable;
    HashTable<const aiMesh*, MeshBoneMatrices*> *meshBoneMatrices;
private:
	void addNodeToTable(RenderNode *node);
	RenderNode* createRenderNode(const aiScene *scene, aiNode *node, RenderNode *parent);
	void updateGlobalTransforms(RenderNode *node);
	void applyNodeTransform(Matrix4f &globalInverse, RenderNode *node, aiMesh *mesh, Vector4f *transformedVerts, Vector4f *transformedNormals);
	GLAssimpBinding *assimpBinding;
    BOOL32 sceneHasBones;
};

}

#endif

#endif /* ASSIMPANIMATOR_H_ */
