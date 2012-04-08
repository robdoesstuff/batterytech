/*
 * GLObjSceneBinding.cpp
 *
 *  Created on: Jul 12, 2011
 *      Author: rgreen
 */

#include "GLObjSceneBinding.h"
#include "GLModelBinding.h"
#include "../util/strx.h"
#include "../importers/obj/ObjImporter.h"
#include "../importers/obj/ObjScene.h"
#include "../Context.h"
#include "../platform/platformgeneral.h"
#include <stdio.h>
#include <stdlib.h>
#include "../Logger.h"

namespace BatteryTech {

GLObjSceneBinding::GLObjSceneBinding(const char *objAssetName) {
	this->objAssetName = strDuplicate(objAssetName);
	modelBindingArray = NULL;
	objScene = NULL;
}

GLObjSceneBinding::~GLObjSceneBinding() {
	delete [] objAssetName;
	objAssetName = NULL;
	if (modelBindingArray) {
		modelBindingArray->deleteElements();
	}
	delete modelBindingArray;
	modelBindingArray = NULL;
	if (objScene) {
		delete objScene;
	}
	objScene = NULL;
}

void GLObjSceneBinding::load(Context *context) {
	if (!objScene) {
		ObjImporter importer;
		char *objData = _platform_load_text_asset(objAssetName);
		if (objData) {
			objScene = importer.import(objData);
			_platform_free_asset((unsigned char*)objData);
		} else {
			char buf[1024];
			sprintf(buf, "Unable to read or open %s", objAssetName);
			logmsg(buf);
		}
	}
	if (objScene) {
		S32 bindingCount = 0;
		for (S32 i = 0; i < objScene->groupCount; i++) {
			ObjGroup *group = &objScene->groups[i];
			bindingCount += group->meshCount;
		}
		if (modelBindingArray) {
			modelBindingArray->deleteElements();
			if (modelBindingArray->capacity < bindingCount) {
				delete modelBindingArray;
				modelBindingArray = new ManagedArray<GLModelBinding>(bindingCount);
			}
		} else {
			modelBindingArray = new ManagedArray<GLModelBinding>(bindingCount);
		}
		// we'll have multiple bindings with the same name - each representing a different mesh for the group
		for (S32 i = 0; i < objScene->groupCount; i++) {
			ObjGroup *group = &objScene->groups[i];
			for (S32 j = 0; j < group->meshCount; j++) {
				ObjMesh *mesh = &group->meshes[j];
				GLModelBinding *binding = new GLModelBinding();
				binding->name = strDuplicate(group->name);
				binding->init(mesh->vertPositions, mesh->normals, mesh->uvs, mesh->vertCount);
				modelBindingArray->add(binding);
			}
		}
	}
}

void GLObjSceneBinding::unload(Context *context) {
	if (objScene) {
		delete objScene;
		objScene = NULL;
	}
	if (modelBindingArray) {
		for (S32 i = 0; i < modelBindingArray->getSize(); i++) {
			modelBindingArray->array[i]->clearGL();
		}
		modelBindingArray->deleteElements();
	}
}

}
