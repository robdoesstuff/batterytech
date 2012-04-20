/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
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

AssimpAnimator::AssimpAnimator() {
	rootNode = NULL;
	scene = NULL;
	nodeTable = NULL;
	nodeCount = 0;
	boneMatrices = NULL;
	boneMatricesWithRootInv = NULL;
	boneMatrixCount = 0;
}

AssimpAnimator::~AssimpAnimator() {
	if (rootNode) {
		delete rootNode;
	}
	rootNode = NULL;
	delete nodeTable;
	nodeTable = NULL;
	delete [] boneMatrices;
	delete [] boneMatricesWithRootInv;
	delete boneMatrixIdxTable;
}

void AssimpAnimator::init(const aiScene *scene, const char *meshName) {
	this->scene = scene;
	aiMesh *mesh = NULL;
	if (meshName) {
		for (U32 i = 0; i < scene->mNumMeshes; i++) {
			aiMesh *m = scene->mMeshes[i];
			if (strEquals(m->mName.data, meshName)) {
				mesh = m;
				break;
			}
		}
	} else {
		// no meshname specified - just use the first one.
		mesh = scene->mMeshes[0];
	}
	if (mesh) {
		// do bone matrix index first because nodes will reference it
		U32 boneCount = mesh->mNumBones;
		boneMatrixIdxTable = new StrHashTable<S32>(boneCount * 1.5f);
		for (U32 i = 0; i < boneCount; i++) {
			boneMatrixIdxTable->put(mesh->mBones[i]->mName.data, (S32)i);
		}
		boneMatrices = new Matrix4f[boneCount];
		boneMatrixCount = boneCount;
		boneMatricesWithRootInv = new Matrix4f[boneCount];
		nodeCount = 0;
		rootNode = createRenderNode(scene, scene->mRootNode, NULL, mesh);
		nodeTable = new StrHashTable<RenderNode*>(nodeCount * 1.5f);
		addNodeToTable(rootNode);
		updateGlobalTransforms(rootNode);
	} else {
		char buf[255];
		sprintf(buf, "Unable to find mesh (%s) - Animator not initialized!", meshName);
		logmsg(buf);
	}
}

void AssimpAnimator::addNodeToTable(RenderNode *node) {
	nodeTable->put(node->name, node);
	for (S32 i = 0; i < node->childNodes->getSize(); i++) {
		addNodeToTable(node->childNodes->array[i]);
	}
}

RenderNode* AssimpAnimator::createRenderNode(const aiScene *scene, aiNode *node, RenderNode *parent, aiMesh *mesh) {
	nodeCount++;
	RenderNode *bNode = new RenderNode();
	bNode->name = strDuplicate(node->mName.data);
	bNode->parentNode = parent;
	bNode->meshCount = node->mNumMeshes;
	bNode->meshIndices = new U32[bNode->meshCount];
	AABB3f aabb;
	for (U32 i = 0; i < node->mNumMeshes; i++) {
		bNode->meshIndices[i] = node->mMeshes[i];
		//determine local AABB - this could take a while on really large meshes.
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
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
	for (U32 i = 0; i < mesh->mNumBones; i++) {
		if (strEquals(mesh->mBones[i]->mName.data, bNode->name)) {
			m = mesh->mBones[i]->mOffsetMatrix;
			bNode->boneOffset = Matrix4f(m.a1, m.b1, m.c1, m.d1, m.a2, m.b2, m.c2, m.d2, m.a3, m.b3, m.c3, m.d3, m.a4, m.b4, m.c4, m.d4);
			bNode->isBone = TRUE;
			bNode->matrixIdx = boneMatrixIdxTable->get(bNode->name);
		}
	}
	bNode->childNodes = new ManagedArray<RenderNode>(node->mNumChildren);
	for (U32 i = 0; i < node->mNumChildren; i++) {
		aiNode *child = node->mChildren[i];
		bNode->childNodes->add(createRenderNode(scene, child, bNode, mesh));
	}
	return bNode;
}

void AssimpAnimator::updateGlobalTransforms(RenderNode *node) {
	if (node->parentNode) {
		node->globalTransform = node->parentNode->globalTransform * node->localTransform;
	} else {
		node->globalTransform = node->localTransform;
	}
	if (node->isBone) {
		boneMatrices[node->matrixIdx] = node->globalTransform * node->boneOffset;
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
	Matrix4f globalInverse = meshNode->globalTransform.inverse();
	// apply the inverse of the meshNode's global transform to our bone matrices
	for (U32 i = 0; i < mesh->mNumBones; i++) {
		boneMatricesWithRootInv[i] = globalInverse * boneMatrices[i];
	}
	return boneMatricesWithRootInv;
}

void AssimpAnimator::applyTransforms(RenderNode *meshNode, const aiMesh *mesh, GLAssimpSkinnedMeshVertex *vertAtts) {
	if (boneMatrixCount == 0) {
		return;
	}
	updateGlobalInverseBoneMatrices(meshNode, mesh);
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
	// apply bone influences by bone instead of by vertex (this can have any number of weights but is hard to move into a shader).
	for (U32 i = 0; i < mesh->mNumBones; i++) {
		aiBone *bone = mesh->mBones[i];
		// first determine - is this a bone node?
		if (strEquals(bone->mName.data, node->name)) {
			//Matrix4f boneMatrix = globalInverse * node->globalTransform * node->boneOffset;
			Matrix4f boneMatrix = globalInverse * boneMatrices[node->matrixIdx];
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
