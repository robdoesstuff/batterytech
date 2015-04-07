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
// Name        : ObjScene.cpp
// Description : An OpenGL-compatible representation of an OBJ file
//============================================================================

#include "ObjScene.h"

#include <string.h>

namespace BatteryTech {

ObjScene::ObjScene() {
	groups = NULL;
}

ObjScene::~ObjScene() {
	for (S32 i = 0; i < groupCount; i++) {
		ObjGroup *group = &groups[i];
		group->meshCount = group->meshCount;
		for (S32 j = 0; j < group->meshCount; j++) {
			ObjMesh *mesh = &group->meshes[j];
			mesh->vertPositions = NULL;
			mesh->normals = NULL;
			mesh->uvs = NULL;
			delete [] mesh->materialName;
		}
		delete [] group->name;
		delete [] group->meshes;
	}
	delete [] groups;
	delete [] vertPositions;
	delete [] normals;
	delete [] uvs;
}

ObjGroup* ObjScene::getGroup(const char *name) {
	for (S32 i = 0; i < groupCount; i++) {
		if (strcmp(groups[i].name, name) == 0) {
			return &groups[i];
		}
	}
	return NULL;
}

void ObjScene::log() {
}

S32 ObjScene::getMemoryConsumed() {
	S32 memoryConsumed = sizeof(ObjScene);
	for (S32 i = 0; i < groupCount; i++) {
		ObjGroup *group = &groups[i];
		for (S32 j = 0; j  < group->meshCount; j++) {
			ObjMesh *mesh = &group->meshes[j];
			// vert positions and normals
			memoryConsumed += mesh->vertCount * 3 * 4 * 2;
			// uvs
			memoryConsumed += mesh->vertCount * 2 * 4;
			memoryConsumed += strlen(mesh->materialName);
		}
		memoryConsumed += sizeof(ObjMesh) * group->meshCount;
		memoryConsumed += strlen(group->name);
	}
	memoryConsumed += sizeof(ObjGroup) * groupCount;
	return memoryConsumed;
}

}
