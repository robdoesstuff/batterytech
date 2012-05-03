/*
 * BTAssimpImporter.cpp
 *
 *  Created on: Oct 12, 2011
 *      Author: rgreen
 */

#include "BTAssimpImporter.h"
#include "../../primitives.h"
#include "../../platform/platformgeneral.h"
#include "../../Logger.h"
#include <stdio.h>
#ifdef BATTERYTECH_INCLUDE_ASSIMP

using namespace Assimp;
#include "../../../bt-assimp/include/aiPostProcess.h"
#include "BTIOSystem.h"


namespace BatteryTech {

BTAssimpImporter::BTAssimpImporter() {
	importedAssetBasename = NULL;
}

BTAssimpImporter::~BTAssimpImporter() {
	delete [] importedAssetBasename;
}

Importer* BTAssimpImporter::importAsset(const char* assetName) {
	char buf[1024];
	sprintf(buf, "Importing Asset [%s]", assetName);
	logmsg(buf);
	//S32 size;
	//unsigned char *data = _platform_load_asset(assetName, &size);
	//const aiScene *scene = importer->ReadFileFromMemory(data, size, aiProcess_LimitBoneWeights, "");
	//const aiScene *scene =  aiImportFile(assetName, aiProcess_LimitBoneWeights);
	Importer *importer = new Importer();
	char convertedAssetName[1024];
	_platform_convert_path(assetName, convertedAssetName);
	importedAssetBasename = new char[1024];
	_platform_get_basename(convertedAssetName, importedAssetBasename);
	char fileName[1024];
	strcpy(fileName, convertedAssetName+strlen(importedAssetBasename)+1);
	// flip it back to the standard forward-slash
	_platform_convert_path_to_forward(importedAssetBasename, importedAssetBasename);
	importer->SetIOHandler(new BTIOSystem(importedAssetBasename));
	const aiScene *scene = importer->ReadFile(fileName, aiProcess_LimitBoneWeights);
	if (scene) {
		//sprintf(buf, "(Scene) Meshes=%d Animations=%d", scene->mNumMeshes, scene->mNumAnimations);
		//logmsg(buf);
		S32 totalVerts = 0;
		S32 totalBones = 0;
		for (U32 i = 0; i < scene->mNumMeshes; i++) {
			aiMesh *mesh = scene->mMeshes[i];
			//sprintf(buf, "(Scene) (Mesh %d) Name=%s Verts=%d Bones=%d", i, mesh->mName.data, mesh->mNumVertices, mesh->mNumBones);
			//logmsg(buf);
			totalVerts += mesh->mNumVertices;
			totalBones += mesh->mNumBones;
			/*
			for (U32 j = 0; j < mesh->mNumBones; j++) {
				aiBone *bone = mesh->mBones[j];
				sprintf(buf, "(Scene) (Mesh %d) (Bone %d) Name=%s Weights=%d", i, j, bone->mName.data, bone->mNumWeights);
				logmsg(buf);
			}
			*/
		}
		/*
		for (U32 i = 0; i < scene->mNumAnimations; i++) {
			aiAnimation *anim = scene->mAnimations[i];
			sprintf(buf, "(Scene) (Animation %d) Channels=%d MeshChannels=%d Duration=%f TPS=%f", i, anim->mNumChannels, anim->mNumMeshChannels, anim->mDuration, anim->mTicksPerSecond);
			logmsg(buf);
			for (U32 j = 0; j < anim->mNumChannels; j++) {
				aiNodeAnim *nodeAnim = anim->mChannels[j];
				sprintf(buf, "(Scene) (Animation %d) (Channel %d) NodeName=%s posKeys=%d, rotKeys=%d, scaleKeys=%d, time[0]=%f, time[1]=%f", i, j, nodeAnim->mNodeName.data, nodeAnim->mNumPositionKeys, nodeAnim->mNumRotationKeys, nodeAnim->mNumScalingKeys, nodeAnim->mPositionKeys[0].mTime, nodeAnim->mPositionKeys[1].mTime);
				logmsg(buf);
			}
		}
		*/
		sprintf(buf, "Assimp Scene loaded: [%d] meshes, [%d] verts, [%d] bones, [%d] animations", scene->mNumMeshes, totalVerts, totalBones, scene->mNumAnimations);
		logmsg(buf);
		// convert to Z-Up
		scene->mRootNode->mTransformation *= aiMatrix4x4(
				 1,  0,  0,  0,
				 0,  0,  -1,  0,
				 0, 1,  0,  0,
				 0,  0,  0,  1);
		// processNode(scene->mRootNode);
		//_platform_free_asset(data);
	} else {
		logmsg("No scene!");
	}
	return importer;
}

void BTAssimpImporter::processNode(aiNode *node) {
	char buf[1024];
	sprintf(buf, "(Scene) (Node) Name=%s Meshes=%d Children=%d", node->mName.data, node->mNumMeshes, node->mNumChildren);
	logmsg(buf);
	for (U32 i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i]);
	}

}

}

#endif
