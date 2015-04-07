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
// Name        : AssimpAnimator.cpp
// Description : Open Asset Importer Scene Animator
//============================================================================

#include "AssimpAnimator.h"
#include <batterytech/util/strx.h>
#include <stdio.h>
#include <batterytech/Logger.h>

#ifdef BATTERYTECH_INCLUDE_ASSIMP

namespace BatteryTech {

/**
 This animator creates a node structure that builds a single set of nodes with various data from the aiScene.
 Bone Nodes are marked and have extra data for each mesh, as some files share different bone nodes for different meshes
 A complex structure might look like
 RootNode
 - ObjectBase
 -- (mesh1 b) Object Hip (mesh2 b) Object Hip
 -- (mesh1 b) Object L-Leg (mesh3 b) Object L-Leg
 
 */
AssimpAnimator::AssimpAnimator() {
	rootNode = NULL;
	assimpBinding = NULL;
	nodeTable = NULL;
	nodeCount = 0;
    meshBoneMatrices = NULL;
    sceneHasBones = FALSE;
}

AssimpAnimator::~AssimpAnimator() {
	if (rootNode) {
		delete rootNode;
	}
	rootNode = NULL;
	delete nodeTable;
	nodeTable = NULL;
	assimpBinding = NULL;
}

void AssimpAnimator::init(GLAssimpBinding *assimpBinding, const char *meshName) {
	this->assimpBinding = assimpBinding;
    /*
	if (meshName) {
		for (U32 i = 0; i < scene->mNumMeshes; i++) {
			aiMesh *m = scene->mMeshes[i];
			if (strEquals(m->mName.data, meshName)) {
				mesh = m;
				break;
			}
		}
	}*/
	const aiScene *scene = assimpBinding->scene;
    meshBoneMatrices = new HashTable<const aiMesh*, MeshBoneMatrices*>(scene->mNumMeshes * 1.5f);
    for (U32 meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++) {
        aiMesh *mesh = scene->mMeshes[meshIdx];
        MeshBoneMatrices *mbMats = new MeshBoneMatrices;
        meshBoneMatrices->put(mesh, mbMats);
		// do bone matrix index first because nodes will reference it
		U32 boneCount = mesh->mNumBones;
        if (boneCount) {
            sceneHasBones = TRUE;
        }
        mbMats->boneMatrixIdxTable = new StrHashTable<S32>(boneCount * 1.5f);
        mbMats->boneMatrixIdxTable->setNullReturnVal(-1);
		for (U32 i = 0; i < boneCount; i++) {
			mbMats->boneMatrixIdxTable->put(mesh->mBones[i]->mName.data, (S32)i);
		}
		mbMats->boneMatrices = new Matrix4f[boneCount];
		mbMats->boneMatrixCount = boneCount;
		mbMats->boneMatricesWithRootInv = new Matrix4f[boneCount];
	}
    nodeCount = 0;
    rootNode = createRenderNode(scene, scene->mRootNode, NULL);
    nodeTable = new StrHashTable<RenderNode*>(nodeCount * 1.5f);
    addNodeToTable(rootNode);
    updateGlobalTransforms(rootNode);
}

void AssimpAnimator::addNodeToTable(RenderNode *node) {
	nodeTable->put(node->name, node);
	for (S32 i = 0; i < node->childNodes->getSize(); i++) {
		addNodeToTable(node->childNodes->array[i]);
	}
}

RenderNode* AssimpAnimator::createRenderNode(const aiScene *scene, aiNode *node, RenderNode *parent) {
	nodeCount++;
	RenderNode *bNode = new RenderNode();
	bNode->name = strDuplicate(node->mName.data);
	bNode->parentNode = parent;
	bNode->meshCount = node->mNumMeshes;
	bNode->meshIndices = new U32[bNode->meshCount];
    if (sceneHasBones) {
        // because we're sharing nodes and bones, we need to prepare for as many bone entries as there are meshes in the whole scene
        // I personally do not like how much memory this takes up (nodes*meshes*sizeof(RenderNodeMeshBone))
        // But it is convenient to have everything in a single node tree
        // The other alernative is to have a global "render" node tree and then a bone tree for each mesh, built in a similar fashion to here
        bNode->meshBones = new RenderNodeMeshBone[scene->mNumMeshes];
        // set meshbones to idx
        for (U32 i = 0; i < scene->mNumMeshes; i++) {
            bNode->meshBones[i].isUsed = FALSE;
            bNode->meshBones[i].boneMeshIdx = i;
            bNode->meshBones[i].matrixIdx = -1;
        }
    } else {
        bNode->meshBones = NULL;
    }
	AABB3f aabb;
    // search all bones of all meshes to see if we have a name match
    for (U32 meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++) {
        aiMesh *mesh = scene->mMeshes[meshIdx];
        // is this a bone node for a mesh?
        MeshBoneMatrices *mbMats = meshBoneMatrices->get(mesh);
        for (U32 boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++) {
            if (strEquals(mesh->mBones[boneIdx]->mName.data, bNode->name)) {
                aiMatrix4x4 m = mesh->mBones[boneIdx]->mOffsetMatrix;
                bNode->isBone = TRUE;
                bNode->meshBones[meshIdx].isUsed = TRUE;
                bNode->meshBones[meshIdx].boneOffset = Matrix4f(m.a1, m.b1, m.c1, m.d1, m.a2, m.b2, m.c2, m.d2, m.a3, m.b3, m.c3, m.d3, m.a4, m.b4, m.c4, m.d4);
                bNode->meshBones[meshIdx].matrixIdx = mbMats->boneMatrixIdxTable->get(bNode->name);
                // char buf[1024];
                // sprintf(buf, "bNode %s meshIdx %d matrixIdx %d", bNode->name, bNode->boneMeshIdx, bNode->matrixIdx);
                // logmsg(buf);
            }
        }
    }
    // now if this node has a mesh, calc the AABB of the mesh at this node's transform
	for (U32 i = 0; i < node->mNumMeshes; i++) {
		bNode->meshIndices[i] = node->mMeshes[i];
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		//determine local AABB - this could take a while on really large meshes.
		for (U32 j = 0; j < mesh->mNumVertices; j++) {
			aiVector3D vert = mesh->mVertices[j];
			if (i == 0 && j == 0) {
				aabb.pMin = Vector3f(vert.x, vert.y, vert.z);
				aabb.pMax = Vector3f(vert.x, vert.y, vert.z);
			}
			aabb.includePoint(vert.x, vert.y, vert.z);
		}
		bNode->localAABB = aabb;
		//char buf[1024];
		//sprintf(buf, "Node AABB = (%f, %f, %f) (%f, %f, %f)", aabbMin.x, aabbMin.y, aabbMin.z, aabbMax.x, aabbMax.y, aabbMax.z);
		///logmsg(buf);
	}
	// convert from row-major to column-major
	aiMatrix4x4 m = node->mTransformation;
	bNode->localTransform = Matrix4f(m.a1, m.b1, m.c1, m.d1, m.a2, m.b2, m.c2, m.d2, m.a3, m.b3, m.c3, m.d3, m.a4, m.b4, m.c4, m.d4);
	bNode->childNodes = new ManagedArray<RenderNode>(node->mNumChildren);
	for (U32 i = 0; i < node->mNumChildren; i++) {
		aiNode *child = node->mChildren[i];
		bNode->childNodes->add(createRenderNode(scene, child, bNode));
	}
	return bNode;
}

void AssimpAnimator::updateGlobalTransforms(RenderNode *node) {
	const aiScene *scene = assimpBinding->scene;
	if (node->parentNode) {
		node->globalTransform = node->parentNode->globalTransform * node->localTransform;
	} else {
		node->globalTransform = node->localTransform;
	}
	if (node->isBone) {
        for (U32 i = 0; i < scene->mNumMeshes; i++) {
            if (node->meshBones[i].isUsed) {
                aiMesh *mesh = scene->mMeshes[node->meshBones[i].boneMeshIdx];
                MeshBoneMatrices *mbMats = meshBoneMatrices->get(mesh);
                mbMats->boneMatrices[node->meshBones[i].matrixIdx] = node->globalTransform * node->meshBones[i].boneOffset;
            }
        }
	}
	node->globalAABB = node->globalTransform * node->localAABB;
	node->globalAABB.reCalc();
	for (S32 i = 0; i < node->childNodes->getSize(); i++) {
		updateGlobalTransforms(node->childNodes->array[i]);
	}
}

void AssimpAnimator::interpolate(F32 pTime) {
	// find closest frames for time
	// update transforms to match
    if (!assimpBinding) {
        logmsg("Unable to interpolate animation - animator not initialized");
        return;
    }
	const aiScene *scene = assimpBinding->scene;
    if (!scene) {
        logmsg("Unable to interpolate animation - binding has no scene");
        return;
    }
	// TODO - support multiple animations!
	aiAnimation *anim = scene->mAnimations[0];
	// extract ticks per second. Assume default value if not given
	F32 ticksPerSecond = anim->mTicksPerSecond != 0.0 ? anim->mTicksPerSecond : 25.0;
	// every following time calculation happens in ticks
	pTime *= ticksPerSecond;
	// map into anim's duration
	F32 time = 0.0f;
	if( anim->mDuration > 0.0) {
		time = fmod(pTime, (F32)anim->mDuration);
	}
	// now just use time
	U32 numChannels = anim->mNumChannels;
	for (U32 i = 0; i < numChannels; i++) {
		const aiNodeAnim* channel = anim->mChannels[i];
		// ******** Position *****
		aiVector3D presentPosition( 0, 0, 0);
		if( channel->mNumPositionKeys > 0) {
			// Look for present frame number. Search from last position if time is after the last time, else from beginning
			// Should be much quicker than always looking from start for the average use case.
			//TODO - use last frame for search optimization!
			//U32 frame = (time >= mLastTime) ? mLastPositions[a].get<0>() : 0;
			U32 frame = 0;
			while( frame < channel->mNumPositionKeys - 1) {
				if( time < channel->mPositionKeys[frame+1].mTime)
					break;
				frame++;
			}
			// interpolate between this frame's value and next frame's value
			U32 nextFrame = (frame + 1) % channel->mNumPositionKeys;
			const aiVectorKey& key = channel->mPositionKeys[frame];
			const aiVectorKey& nextKey = channel->mPositionKeys[nextFrame];
			F32 diffTime = nextKey.mTime - key.mTime;
			if( diffTime < 0.0) {
				diffTime += anim->mDuration;
			}
			if( diffTime > 0) {
				F32 factor = float( (time - key.mTime) / diffTime);
				presentPosition = key.mValue + (nextKey.mValue - key.mValue) * factor;
			} else{
				presentPosition = key.mValue;
			}
			//mLastPositions[a].get<0>() = frame;
		}

		// ******** Rotation *********
		aiQuaternion presentRotation( 1, 0, 0, 0);
		if( channel->mNumRotationKeys > 0) {
			//U32 frame = (time >= mLastTime) ? mLastPositions[a].get<1>() : 0;
			U32 frame = 0;
			while( frame < channel->mNumRotationKeys - 1) {
				if( time < channel->mRotationKeys[frame+1].mTime)
					break;
				frame++;
			}
			// interpolate between this frame's value and next frame's value
			U32 nextFrame = (frame + 1) % channel->mNumRotationKeys;
			const aiQuatKey& key = channel->mRotationKeys[frame];
			const aiQuatKey& nextKey = channel->mRotationKeys[nextFrame];
			F32 diffTime = nextKey.mTime - key.mTime;
			if( diffTime < 0.0) {
				diffTime += anim->mDuration;
			}
			if( diffTime > 0) {
				F32 factor = float( (time - key.mTime) / diffTime);
				aiQuaternion::Interpolate( presentRotation, key.mValue, nextKey.mValue, factor);
			} else {
				presentRotation = key.mValue;
			}
			//mLastPositions[a].get<1>() = frame;
		}

		// ******** Scaling **********
		aiVector3D presentScaling( 1, 1, 1);
		if( channel->mNumScalingKeys > 0) {
			//U32 frame = (time >= mLastTime) ? mLastPositions[a].get<2>() : 0;
			U32 frame = 0;
			while( frame < channel->mNumScalingKeys - 1) {
				if( time < channel->mScalingKeys[frame+1].mTime) {
					break;
				}
				frame++;
			}
			// TODO: (thom) interpolation maybe? This time maybe even logarithmic, not linear
			presentScaling = channel->mScalingKeys[frame].mValue;
			//mLastPositions[a].get<2>() = frame;
		}
		// build a transformation matrix from it
		aiMatrix4x4 m = aiMatrix4x4( presentRotation.GetMatrix());
		m.a1 *= presentScaling.x; m.b1 *= presentScaling.x; m.c1 *= presentScaling.x;
		m.a2 *= presentScaling.y; m.b2 *= presentScaling.y; m.c2 *= presentScaling.y;
		m.a3 *= presentScaling.z; m.b3 *= presentScaling.z; m.c3 *= presentScaling.z;
		m.a4 = presentPosition.x; m.b4 = presentPosition.y; m.c4 = presentPosition.z;
		RenderNode *node = nodeTable->get(channel->mNodeName.data);
		if (node) {
			node->localTransform = Matrix4f(m.a1, m.b1, m.c1, m.d1, m.a2, m.b2, m.c2, m.d2, m.a3, m.b3, m.c3, m.d3, m.a4, m.b4, m.c4, m.d4);
		}
	}
	updateGlobalTransforms(rootNode);
//	mLastTime = time;
}

Matrix4f* AssimpAnimator::updateGlobalInverseBoneMatrices(RenderNode *meshNode, const aiMesh *mesh) {
    MeshBoneMatrices *mbMats = meshBoneMatrices->get(mesh);
	Matrix4f globalInverse = meshNode->globalTransform.inverse();
	// apply the inverse of the meshNode's global transform to our bone matrices
	for (U32 i = 0; i < mesh->mNumBones; i++) {
		mbMats->boneMatricesWithRootInv[i] = globalInverse * mbMats->boneMatrices[i];
	}
	return mbMats->boneMatricesWithRootInv;
}

void AssimpAnimator::applyTransforms(RenderNode *meshNode, const aiMesh *mesh, GLAssimpSkinnedMeshVertex *vertAtts) {
    MeshBoneMatrices *mbMats = meshBoneMatrices->get(mesh);
	if (mbMats->boneMatrixCount == 0) {
		return;
	}
	updateGlobalInverseBoneMatrices(meshNode, mesh);
    Matrix4f *boneMatricesWithRootInv = mbMats->boneMatricesWithRootInv;
	// now apply the (global inverse * transform * offset) bone matrices with weight to verts and normals.
	for (U32 i = 0; i < mesh->mNumVertices; i++) {
		const aiVector3D &pv = mesh->mVertices[i];
		const aiVector3D &nv = mesh->mNormals[i];
		Vector4f poseVert = Vector4f(pv.x, pv.y, pv.z, 1.0f);
		Vector4f poseNormal = Vector4f(nv.x, nv.y, nv.z, 0);
		const Vector4b &matIdx = vertAtts[i].bones;
		const Vector4f &weights = vertAtts[i].weights;
		vertAtts[i].position = (boneMatricesWithRootInv[matIdx.x] * poseVert) * weights.x +
				(boneMatricesWithRootInv[matIdx.y] * poseVert) * weights.y +
				(boneMatricesWithRootInv[matIdx.z] * poseVert) * weights.z +
				(boneMatricesWithRootInv[matIdx.w] * poseVert) * weights.w;
		vertAtts[i].normal = (boneMatricesWithRootInv[matIdx.x] * poseNormal) * weights.x +
				(boneMatricesWithRootInv[matIdx.y] * poseNormal) * weights.y +
				(boneMatricesWithRootInv[matIdx.z] * poseNormal) * weights.z +
				(boneMatricesWithRootInv[matIdx.w] * poseNormal) * weights.w;
		vertAtts[i].normal.normalize();
	}
}

void AssimpAnimator::applyNodeTransform(Matrix4f &globalInverse, RenderNode *node, aiMesh *mesh, Vector4f *transformedVerts, Vector4f *transformedNormals) {
	const aiScene *scene = assimpBinding->scene;
    MeshBoneMatrices *mbMats = meshBoneMatrices->get(mesh);
    U32 meshIdx = 0;
    for (U32 i = 0; i < scene->mNumMeshes; i++) {
        if (scene->mMeshes[i] == mesh) {
            meshIdx = i;
            break;
        }
    }
	// apply bone influences by bone instead of by vertex (this can have any number of weights but is hard to move into a shader).
	for (U32 i = 0; i < mesh->mNumBones; i++) {
		aiBone *bone = mesh->mBones[i];
		// first determine - is this a bone node?
		if (strEquals(bone->mName.data, node->name)) {
			//Matrix4f boneMatrix = globalInverse * node->globalTransform * node->boneOffset;
			Matrix4f boneMatrix = globalInverse * mbMats->boneMatrices[node->meshBones[meshIdx].matrixIdx];
			for (U32 j = 0; j < bone->mNumWeights; j++) {
				U32 vertIdx = bone->mWeights[j].mVertexId;
				F32 weight = bone->mWeights[j].mWeight;
				const aiVector3D &pv = mesh->mVertices[vertIdx];
				const aiVector3D &nv = mesh->mNormals[vertIdx];
				transformedVerts[vertIdx] = transformedVerts[vertIdx] + (boneMatrix * Vector4f(pv.x, pv.y, pv.z, 1.0f)) * weight;
				transformedNormals[vertIdx] = transformedNormals[vertIdx] + (boneMatrix * Vector4f(nv.x, nv.y, nv.z, 0)) * weight;
			}
			break;
		}
	}
	// recurse into children
	for (S32 i = 0; i < node->childNodes->getSize(); i++) {
		applyNodeTransform(globalInverse, node->childNodes->array[i], mesh, transformedVerts, transformedNormals);
	}
}

}

#endif
