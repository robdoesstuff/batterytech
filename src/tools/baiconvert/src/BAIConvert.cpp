//============================================================================
// Name        : AssimpTools.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "assimp/include/aiScene.h"
#include "assimp/code/AssimpPCH.h"
#include "assimp/code/BAIFileImporter.h"

using namespace std;
using namespace Assimp;

#define AIVECTORKEYSIZE 24
#define AIQUATKEYSIZE 24
#define AIMESHKEYSIZE 16

void exportBAI(const aiScene *scene, const char *filename);
aiScene* importBAI(const char *filename);

static bai_u32 getDataLength(aiMaterialProperty *prop) {
	return prop->mDataLength + prop->mKey.length;
}

static bai_u32 getDataLength(aiMaterial *mat) {
	bai_u32 propsTotal = 0;
	propsTotal = sizeof(bai_materialheader);
	for (bai_u32 i = 0; i < mat->mNumProperties; i++) {
		propsTotal += sizeof(bai_materialpropheader) + getDataLength(mat->mProperties[i]);
	}
	return propsTotal;
}

static bai_u32 getDataLength(aiBone *bone) {
	return sizeof(bai_meshboneheader) + bone->mName.length + bone->mNumWeights * sizeof(aiVertexWeight) + sizeof(aiMatrix4x4);
}

static bai_u32 getDataLength(aiMesh *mesh) {
	bai_u32 bonesTotal = 0;
	for (bai_u32 i = 0; i < mesh->mNumBones; i++) {
		bonesTotal += getDataLength(mesh->mBones[i]);
	}
	// pos = 12, norm = 12, uv = 8, face = 3 shorts = 6
	bai_u32 length = sizeof(bai_meshheader) + mesh->mNumVertices * sizeof(aiVector3D)*2 + mesh->mNumVertices*8 + mesh->mNumFaces * sizeof(bai_face_triangle) + mesh->mName.length + bonesTotal;
	return length;
}

static bai_u32 getDataLength(aiNode *node) {
	bai_u32 totalLength = sizeof(bai_nodeheader);
	totalLength += node->mNumMeshes * 4;
	totalLength += sizeof(aiMatrix4x4);
	totalLength += node->mName.length;
	for (bai_u32 i = 0; i < node->mNumChildren; i++) {
		totalLength += getDataLength(node->mChildren[i]);
	}
	return totalLength;
}

static bai_u32 getDataLength(aiNodeAnim *channel) {
	bai_u32 totalLength = sizeof(bai_animationchannelheader);
	totalLength += channel->mNodeName.length;
	totalLength += channel->mNumPositionKeys * AIVECTORKEYSIZE;
	totalLength += channel->mNumRotationKeys * AIQUATKEYSIZE;
	totalLength += channel->mNumScalingKeys * AIVECTORKEYSIZE;
	return totalLength;
}

static bai_u32 getDataLength(aiMeshAnim *meshAnim) {
	bai_u32 totalLength = sizeof(bai_animationmeshchannelheader);
	totalLength += meshAnim->mName.length;
	totalLength += meshAnim->mNumKeys * AIMESHKEYSIZE;
	return totalLength;
}

static bai_u32 getDataLength(aiAnimation *anim) {
	bai_u32 totalLength = sizeof(bai_animationheader);
	totalLength += anim->mName.length;
	for (bai_u32 j = 0; j < anim->mNumChannels; j++) {
		totalLength += getDataLength(anim->mChannels[j]);
	}
	for (bai_u32 j = 0; j < anim->mNumMeshChannels; j++) {
		totalLength += getDataLength(anim->mMeshChannels[j]);
	}
	return totalLength;
}

static bai_u32 countNodes(aiNode *node) {
	bai_u32 count = 1;
	for (bai_u32 i = 0; i < node->mNumChildren; i++) {
		count += countNodes(node->mChildren[i]);
	}
	return count;
}

static bai_u32 writeToData(unsigned char *target, bai_u32 offset, void *src, size_t size) {
	memcpy(target+offset, src, size);
	return offset+size;
}

static bai_u32 writeNode(unsigned char *target, bai_u32 offset, aiNode *node) {
	bai_nodeheader nodeHeader;
	nodeHeader.numChildren = node->mNumChildren;
	nodeHeader.numMeshes = node->mNumMeshes;
	nodeHeader.nameLength = node->mName.length;
	bai_u32 nextOffset = offset;
	nextOffset=writeToData(target, nextOffset, &nodeHeader, sizeof(nodeHeader));
	nextOffset=writeToData(target, nextOffset, node->mMeshes, node->mNumMeshes*4);
	nextOffset=writeToData(target, nextOffset, &node->mTransformation, sizeof(aiMatrix4x4));
	nextOffset=writeToData(target, nextOffset, node->mName.data, node->mName.length);
	for (bai_u32 i = 0; i < node->mNumChildren; i++) {
		nextOffset=writeNode(target, nextOffset, node->mChildren[i]);
	}
	return nextOffset;
}

static bai_u32 readFromData(void *target, bai_u32 offset, const unsigned char *src, size_t size) {
	memcpy(target, src+offset, size);
	return offset+size;
}

static bai_u32 readNode(aiNode *node, bai_u32 offset, unsigned char *src) {
	bai_u32 myOffset = offset;
	bai_nodeheader nodeHeader;
	myOffset=readFromData(&nodeHeader, myOffset, src, sizeof(bai_nodeheader));
	node->mChildren = new aiNode*[nodeHeader.numChildren];
	node->mNumChildren = nodeHeader.numChildren;
	node->mNumMeshes = nodeHeader.numMeshes;
	node->mMeshes = new bai_u32[nodeHeader.numMeshes];
	node->mName.length = nodeHeader.nameLength;
	myOffset=readFromData(node->mMeshes, myOffset, src, nodeHeader.numMeshes*4);
	myOffset=readFromData(&node->mTransformation, myOffset, src, sizeof(aiMatrix4x4));
	myOffset=readFromData(node->mName.data, myOffset, src, nodeHeader.nameLength);
	node->mName.data[nodeHeader.nameLength] = '\0';
	// cout << "Found node " << node->mName.data << endl;
	for (bai_u32 i = 0; i < node->mNumChildren; i++) {
		node->mChildren[i] = new aiNode();
		node->mChildren[i]->mParent = node;
		myOffset=readNode(node->mChildren[i], myOffset, src);
	}
	return myOffset;
}

void exportBAI(const aiScene *scene, const char *filename) {
	FILE *f = fopen(filename, "wb");
	bai_header mainHeader;
	mainHeader.token = BAI_MAGIC;
	mainHeader.length = sizeof(bai_header);
	mainHeader.version = 1;
	mainHeader.numMeshes = scene->mNumMeshes;
	mainHeader.numMaterials = scene->mNumMaterials;
	if (!scene->mRootNode) {
		mainHeader.numNodes = 0;
	} else {
		mainHeader.numNodes = countNodes(scene->mRootNode);
	}
	mainHeader.numAnimations = scene->mNumAnimations;
	// aiMeshes
	bai_u32 meshesTotalBytes = 0;
	bai_u32 meshesTotalVerts = 0;
	bai_u32 meshesTotalFaces = 0;
	// total up mesh data size
	mainHeader.firstMeshOffset = mainHeader.length;
	cout << "mLength just header " << mainHeader.length << " bytes" << endl;
	for (bai_u32 i = 0; i < scene->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[i];
		mainHeader.length += getDataLength(mesh);
	}
	cout << "mLength with meshes " << mainHeader.length << " bytes" << endl;
	// total up materials data size
	mainHeader.firstMaterialOffset = mainHeader.length;
	for (bai_u32 i = 0; i < scene->mNumMaterials; i++) {
		aiMaterial *mat = scene->mMaterials[i];
		mainHeader.length += getDataLength(mat);
	}
	cout << "mLength with materials " << mainHeader.length << " bytes" << endl;
	mainHeader.firstNodeOffset = mainHeader.length;
	if (scene->mRootNode) {
		mainHeader.length += getDataLength(scene->mRootNode);
	}
	cout << "mLength with nodes " << mainHeader.length << " bytes" << endl;
	mainHeader.firstAnimationOffset = mainHeader.length;
	for (bai_u32 i = 0; i < scene->mNumAnimations; i++) {
		mainHeader.length += getDataLength(scene->mAnimations[i]);
	}
	unsigned char *data = new unsigned char[mainHeader.length];
	cout << "Allocating " << mainHeader.length << " bytes" << endl;
	memcpy(data, &mainHeader, sizeof(mainHeader));
	bai_u32 offset = sizeof(mainHeader);
	for (bai_u32 i = 0; i < scene->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[i];
		// cout << "Writing out mesh name " << mesh->mName.data << endl;
		bai_meshheader meshHeader;
		// bai_u32 offsetStart = offset;
		// assuming triangle faces
		meshHeader.dataLength = getDataLength(mesh);
		meshHeader.numVerts = mesh->mNumVertices;
		meshHeader.numFaces = mesh->mNumFaces;
		meshHeader.numBones = mesh->mNumBones;
		meshHeader.nameLength = mesh->mName.length;
		meshHeader.materialIndex = mesh->mMaterialIndex;
		meshesTotalBytes += meshHeader.dataLength;
		meshesTotalVerts += meshHeader.numVerts;
		meshesTotalFaces += meshHeader.numFaces;
		offset=writeToData(data, offset, &meshHeader, sizeof(meshHeader));
		offset=writeToData(data, offset, mesh->mVertices, sizeof(aiVector3D)*mesh->mNumVertices);
		offset=writeToData(data, offset, mesh->mNormals, sizeof(aiVector3D)*mesh->mNumVertices);
		for (bai_u32 j = 0; j < mesh->mNumVertices; j++) {
			// why write 3 values when you only need 2?
			offset=writeToData(data, offset, &mesh->mTextureCoords[0][j].x, sizeof(float));
			offset=writeToData(data, offset, &mesh->mTextureCoords[0][j].y, sizeof(float));
		}
		for (bai_u32 j = 0; j < mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];
			bai_face_triangle ft;
			ft.v1 = face.mIndices[0];
			ft.v2 = face.mIndices[1];
			ft.v3 = face.mIndices[2];
			offset=writeToData(data, offset, &ft, sizeof(ft));
		}
		for (bai_u32 j = 0; j < mesh->mNumBones; j++) {
			aiBone *bone = mesh->mBones[j];
			bai_meshboneheader boneHeader;
			boneHeader.influences = bone->mNumWeights;
			boneHeader.nameLength = bone->mName.length;
			offset=writeToData(data, offset, &boneHeader, sizeof(boneHeader));
			offset=writeToData(data, offset, bone->mWeights, sizeof(aiVertexWeight)*bone->mNumWeights);
			offset=writeToData(data, offset, bone->mName.data, bone->mName.length);
			offset=writeToData(data, offset, &bone->mOffsetMatrix, sizeof(aiMatrix4x4));
		}
		offset=writeToData(data, offset, mesh->mName.data, mesh->mName.length);
		// cout << "DataLength of mesh is " << meshHeader.dataLength + sizeof(meshHeader) << " offset delta is " << offset - offsetStart << endl;
	}
	cout << "offset after meshes " << offset << endl;
	cout << "Meshes total bytes = " << meshesTotalBytes << endl;
	cout << "Meshes total verts = " << meshesTotalVerts << endl;
	cout << "Meshes total faces = " << meshesTotalFaces << endl;
	cout << "Materials = " << mainHeader.numMaterials << endl;
	cout << "Nodes = " << mainHeader.numNodes << endl;
	cout << "Textures = " << scene->mNumTextures << endl;
	// aiMaterials
	for (bai_u32 i = 0; i < scene->mNumMaterials; i++) {
		aiMaterial *mat = scene->mMaterials[i];
		bai_materialheader matHeader;
		matHeader.numProperties = mat->mNumProperties;
		offset=writeToData(data, offset, &matHeader, sizeof(matHeader));
		for (bai_u32 j = 0; j < mat->mNumProperties; j++) {
			aiMaterialProperty *prop = mat->mProperties[j];
			bai_materialpropheader propHeader;
			propHeader.dataLength = prop->mDataLength;
			propHeader.keyLength = prop->mKey.length;
			propHeader.index = prop->mIndex;
			propHeader.symantec = prop->mSemantic;
			propHeader.type = prop->mType;
			offset=writeToData(data, offset, &propHeader, sizeof(propHeader));
			offset=writeToData(data, offset, prop->mData, prop->mDataLength);
			offset=writeToData(data, offset, prop->mKey.data, prop->mKey.length);
			// cout << "prop " << prop->mKey.data << " data " << prop->mData << endl;
			// cout << "wrote material " << prop->mKey.data << endl;
		}
	}
	cout << "offset after materials " << offset << endl;
	// aiNodes
	if (scene->mRootNode) {
		offset=writeNode(data, offset, scene->mRootNode);
	}
	cout << "offset after nodes " << offset << endl;
	cout << "animations = " << scene->mNumAnimations << endl;
	// aiAnimations
	for (bai_u32 i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation *anim = scene->mAnimations[i];
		bai_animationheader animationHeader;
		animationHeader.duration = (bai_f32) anim->mDuration;
		animationHeader.ticksPerSecond = (bai_f32) anim->mTicksPerSecond;
		animationHeader.nameLength = anim->mName.length;
		animationHeader.numChannels = anim->mNumChannels;
		animationHeader.numMeshChannels = anim->mNumMeshChannels;
		cout << "animation duration=" << animationHeader.duration << " channels=" << animationHeader.numChannels << " meshChannels=" << animationHeader.numMeshChannels << endl;
		offset=writeToData(data, offset, &animationHeader, sizeof(animationHeader));
		offset=writeToData(data, offset, anim->mName.data, anim->mName.length);
		for (bai_u32 j = 0; j < anim->mNumChannels; j++) {
			aiNodeAnim *channel = anim->mChannels[j];
			bai_animationchannelheader chanHeader;
			chanHeader.nodeNameLength = channel->mNodeName.length;
			chanHeader.numPositionKeys = channel->mNumPositionKeys;
			chanHeader.numRotationKeys = channel->mNumRotationKeys;
			chanHeader.numScalingKeys = channel->mNumScalingKeys;
			chanHeader.preState = channel->mPreState;
			chanHeader.postState = channel->mPostState;
			cout << "animation channel=" << j << " nodeName=" << channel->mNodeName.data << " pks=" << chanHeader.numPositionKeys << " rks=" << chanHeader.numRotationKeys << " sks=" << chanHeader.numScalingKeys << endl;
			offset=writeToData(data, offset, &chanHeader, sizeof(chanHeader));
			for (bai_u32 k = 0; k < channel->mNumPositionKeys; k++) {
				aiVectorKey vKey = channel->mPositionKeys[k];
				offset=writeToData(data, offset, &vKey, AIVECTORKEYSIZE);
			}
			for (bai_u32 k = 0; k < channel->mNumRotationKeys; k++) {
				aiQuatKey qKey = channel->mRotationKeys[k];
				offset=writeToData(data, offset, &qKey, AIQUATKEYSIZE);
			}
			for (bai_u32 k = 0; k < channel->mNumScalingKeys; k++) {
				aiVectorKey vKey = channel->mScalingKeys[k];
				offset=writeToData(data, offset, &vKey, AIVECTORKEYSIZE);
			}
			offset=writeToData(data, offset, channel->mNodeName.data, channel->mNodeName.length);
		}
		for (bai_u32 j = 0; j < anim->mNumMeshChannels; j++) {
			bai_animationmeshchannelheader meshChanHeader;
			aiMeshAnim *meshAnim = anim->mMeshChannels[j];
			meshChanHeader.numKeys = meshAnim->mNumKeys;
			offset=writeToData(data, offset, &meshChanHeader, sizeof(meshChanHeader));
			offset=writeToData(data, offset, meshAnim->mName.data, meshAnim->mName.length);
			for (bai_u32 k = 0; k < meshAnim->mNumKeys; k++) {
				aiMeshKey meshKey = meshAnim->mKeys[k];
				offset=writeToData(data, offset, &meshKey, AIMESHKEYSIZE);
			}
		}
	}
	cout << "Length should be " << mainHeader.length << endl;
	cout << "Offset is now " << offset << endl;
	fwrite(data, mainHeader.length, 1, f);
	delete [] data;
	fclose(f);
}

aiScene* importBAI(const char *filename) {
	cout << "********** Import Test ****************" << endl;
	aiScene *scene = new aiScene();
	FILE *f = fopen(filename, "rb");
	bai_header mainHeader;
	fread(&mainHeader, sizeof(mainHeader), 1, f);
	if (mainHeader.token == BAI_MAGIC) {
		cout << "magic number matches" << endl;
	} else {
		cout << "magic number mismatch" << endl;
		return NULL;
	}
	cout << "data length = " << mainHeader.length << endl;
	cout << "version = " << mainHeader.version << endl;
	cout << "numMeshes = " << mainHeader.numMeshes << endl;
	cout << "numMaterials = " << mainHeader.numMaterials << endl;
	cout << "numNodes = " << mainHeader.numNodes << endl;
	cout << "numAnimations = " << mainHeader.numAnimations << endl;
	unsigned char *data = new unsigned char[mainHeader.length - sizeof(mainHeader)];
	fread(data, mainHeader.length - sizeof(mainHeader), 1, f);
	fclose(f);
	bai_u32 offset = 0;
	scene->mMeshes = new aiMesh*[mainHeader.numMeshes];
	for (bai_u32 i = 0; i < mainHeader.numMeshes; i++) {
		bai_meshheader meshHeader;
		offset=readFromData(&meshHeader, offset, data, sizeof(bai_meshheader));
		aiMesh *mesh = new aiMesh();
		mesh->mNumVertices = meshHeader.numVerts;
		mesh->mNumFaces = meshHeader.numFaces;
		mesh->mNumBones = meshHeader.numBones;
		mesh->mVertices = new aiVector3D[meshHeader.numVerts];
		mesh->mNormals = new aiVector3D[meshHeader.numVerts];
		mesh->mTextureCoords[0] = new aiVector3D[meshHeader.numVerts];
		mesh->mMaterialIndex = meshHeader.materialIndex;
		offset=readFromData(mesh->mVertices, offset, data, meshHeader.numVerts*sizeof(aiVector3D));
		offset=readFromData(mesh->mNormals, offset, data, meshHeader.numVerts*sizeof(aiVector3D));
		for (bai_u32 j = 0; j < mesh->mNumVertices; j++) {
			// read 2 UV values back into 3-value element
			offset=readFromData(&mesh->mTextureCoords[0][j].x, offset, data, sizeof(float));
			offset=readFromData(&mesh->mTextureCoords[0][j].y, offset, data, sizeof(float));
			mesh->mTextureCoords[0][j].z = 0;
		}
		mesh->mFaces = new aiFace[mesh->mNumFaces];
		for (bai_u32 j = 0; j < mesh->mNumFaces; j++) {
			bai_face_triangle ft;
			offset=readFromData(&ft, offset, data, sizeof(ft));
			aiFace *face = &mesh->mFaces[j];
			face->mNumIndices = 3;
			face->mIndices = new unsigned int[3];
			face->mIndices[0] = ft.v1;
			face->mIndices[1] = ft.v2;
			face->mIndices[2] = ft.v3;
		}
		mesh->mBones = new aiBone*[mesh->mNumBones];
		for (bai_u32 j = 0; j < mesh->mNumBones; j++) {
			bai_meshboneheader boneHeader;
			offset=readFromData(&boneHeader, offset, data, sizeof(bai_meshboneheader));
			aiBone *bone = new aiBone;
			mesh->mBones[j] = bone;
			bone->mNumWeights = boneHeader.influences;
			bone->mWeights = new aiVertexWeight[bone->mNumWeights];
			bone->mName.length = boneHeader.nameLength;
			offset=readFromData(bone->mWeights, offset, data, sizeof(aiVertexWeight)*bone->mNumWeights);
			offset=readFromData(bone->mName.data, offset, data, boneHeader.nameLength);
			bone->mName.data[boneHeader.nameLength] = '\0';
			offset=readFromData(&bone->mOffsetMatrix, offset, data, sizeof(aiMatrix4x4));
			mesh->mBones[j] = bone;
		}
		offset=readFromData(mesh->mName.data, offset, data, meshHeader.nameLength);
		mesh->mName.length = meshHeader.nameLength;
		mesh->mName.data[meshHeader.nameLength] = '\0';
		// cout << "Read in mesh name " << mesh->mName.data << endl;
		scene->mMeshes[i] = mesh;
	}
	// aiMaterials
	scene->mNumMaterials = mainHeader.numMaterials;
	scene->mMaterials = new aiMaterial*[mainHeader.numMaterials];
	for (bai_u32 i = 0; i < mainHeader.numMaterials; i++) {
		aiMaterial *mat = new aiMaterial();
		bai_materialheader matHeader;
		offset=readFromData(&matHeader, offset, data, sizeof(matHeader));
		mat->mNumAllocated = matHeader.numProperties;
		mat->mNumProperties = matHeader.numProperties;
		mat->mProperties = new aiMaterialProperty*[matHeader.numProperties];
		for (bai_u32 j = 0; j < mat->mNumProperties; j++) {
			aiMaterialProperty *prop = new aiMaterialProperty();
			bai_materialpropheader propHeader;
			offset=readFromData(&propHeader, offset, data, sizeof(propHeader));
			prop->mDataLength = propHeader.dataLength;
			prop->mIndex = propHeader.index;
			prop->mSemantic = propHeader.symantec;
			prop->mType = propHeader.type;
			prop->mData = new char[prop->mDataLength];
			// put null on end?
			offset=readFromData(prop->mData, offset, data, prop->mDataLength);
			offset=readFromData(prop->mKey.data, offset, data, propHeader.keyLength);
			prop->mKey.length = propHeader.keyLength;
			prop->mKey.data[propHeader.keyLength] = '\0';
			//cout << "read in material " << prop->mKey.data << endl;
			mat->mProperties[j] = prop;
			// cout << "prop " << prop->mKey.data << " data " << prop->mData << endl;
		}
		scene->mMaterials[i] = mat;
	}
	// aiNodes
	if (mainHeader.numNodes > 0) {
		scene->mRootNode = new aiNode();
		offset=readNode(scene->mRootNode, offset, data);
	} else {
		scene->mRootNode = NULL;
	}
	// aiAnimations
	scene->mNumAnimations = mainHeader.numAnimations;
	scene->mAnimations = new aiAnimation*[mainHeader.numAnimations];
	for (bai_u32 i = 0; i < mainHeader.numAnimations; i++) {
		aiAnimation *anim = new aiAnimation;
		scene->mAnimations[i] = anim;
		bai_animationheader animationHeader;
		offset=readFromData(&animationHeader, offset, data, sizeof(bai_animationheader));
		anim->mDuration = animationHeader.duration;
		anim->mTicksPerSecond = animationHeader.ticksPerSecond;
		anim->mNumChannels = animationHeader.numChannels;
		anim->mNumMeshChannels = animationHeader.numMeshChannels;
		offset=readFromData(anim->mName.data, offset, data, animationHeader.nameLength);
		anim->mName.length = animationHeader.nameLength;
		anim->mName.data[animationHeader.nameLength] = '\0';
		anim->mChannels = new aiNodeAnim*[anim->mNumChannels];
		anim->mMeshChannels = new aiMeshAnim*[anim->mNumMeshChannels];
		for (bai_u32 j = 0; j < anim->mNumChannels; j++) {
			aiNodeAnim *channel = new aiNodeAnim;
			anim->mChannels[j] = channel;
			bai_animationchannelheader chanHeader;
			offset=readFromData(&chanHeader, offset, data, sizeof(bai_animationchannelheader));
			channel->mNumPositionKeys = chanHeader.numPositionKeys;
			channel->mNumRotationKeys = chanHeader.numRotationKeys;
			channel->mNumScalingKeys = chanHeader.numScalingKeys;
			channel->mPreState = chanHeader.preState;
			channel->mPostState = chanHeader.postState;
			channel->mPositionKeys = new aiVectorKey[chanHeader.numPositionKeys];
			channel->mRotationKeys = new aiQuatKey[chanHeader.numRotationKeys];
			channel->mScalingKeys = new aiVectorKey[chanHeader.numScalingKeys];
			for (bai_u32 k = 0; k < channel->mNumPositionKeys; k++) {
				offset=readFromData(&channel->mPositionKeys[k], offset, data, AIVECTORKEYSIZE);
			}
			for (bai_u32 k = 0; k < channel->mNumRotationKeys; k++) {
				offset=readFromData(&channel->mRotationKeys[k], offset, data, AIQUATKEYSIZE);
			}
			for (bai_u32 k = 0; k < channel->mNumScalingKeys; k++) {
				offset=readFromData(&channel->mScalingKeys[k], offset, data, AIVECTORKEYSIZE);
			}
			offset=readFromData(channel->mNodeName.data, offset, data, chanHeader.nodeNameLength);
			channel->mNodeName.length = chanHeader.nodeNameLength;
			channel->mNodeName.data[chanHeader.nodeNameLength] = '\0';
		}
		for (bai_u32 j = 0; j < anim->mNumMeshChannels; j++) {
			aiMeshAnim *meshAnim = new aiMeshAnim;
			anim->mMeshChannels[j] = meshAnim;
			bai_animationmeshchannelheader meshChanHeader;
			offset=readFromData(&meshChanHeader, offset, data, sizeof(bai_animationmeshchannelheader));
			meshAnim->mNumKeys = meshChanHeader.numKeys;
			offset=readFromData(meshAnim->mName.data, offset, data, meshChanHeader.nameLength);
			meshAnim->mName.length = meshChanHeader.nameLength;
			meshAnim->mName.data[meshChanHeader.nameLength] = '\0';
			meshAnim->mKeys = new aiMeshKey[meshAnim->mNumKeys];
			for (bai_u32 k = 0; k < meshAnim->mNumKeys; k++) {
				offset=readFromData(&meshAnim->mKeys[k], offset, data, AIMESHKEYSIZE);
			}
		}
	}
	cout << "Read complete expected = " << mainHeader.length << " actual = " << offset + sizeof(mainHeader) << endl;
	delete [] data;
	return scene;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cout << "BatteryTech BAI Converter" << endl << "Usage: " << argv[0] << " <input.dae> <output.bai>" << endl;
		return 1;
	}
	cout << "Converting " << endl; // prints !!!Hello World!!!
	Importer *importer = new Importer();
	const aiScene *scene = importer->ReadFile(argv[1], aiProcess_LimitBoneWeights | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	if (scene) {
		cout << "got a scene" << endl;
		exportBAI(scene, argv[2]);
		//importBAI(argv[2]);
	}
	delete importer;
	cout << "Done!" << endl;
	return 0;
}
