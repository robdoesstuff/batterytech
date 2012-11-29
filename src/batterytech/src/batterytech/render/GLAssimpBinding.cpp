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
// Name        : GLAssimpBinding.cpp
// Description : Open Asset Importer to OpenGL VBO Bindings
//============================================================================

#ifdef BATTERYTECH_INCLUDE_ASSIMP

#include "GLAssimpBinding.h"
#include "../util/strx.h"
#include "../importers/assimp/BTAssimpImporter.h"
#include "../platform/platformgl.h"
#include <batterytech/Logger.h>
#include "GLAssimpMeshBinding.h"
#include "AssimpAnimator.h"

using namespace Assimp;

namespace BatteryTech {

GLAssimpBinding::GLAssimpBinding(const char *objAssetName) {
	this->objAssetName = strDuplicate(objAssetName);
	importer = NULL;
	scene = NULL;
	meshBindings = NULL;
	meshBindingPtrs = NULL;
	importedAssetBasename = NULL;
	defaultAnimator = NULL;
}

GLAssimpBinding::~GLAssimpBinding() {
	if (importer) {
		delete importer;
	}
	importer = NULL;
	scene = NULL;
	if (meshBindings) {
		meshBindings->deleteElements();
		delete meshBindings;
	}
	meshBindings = NULL;
	if (meshBindingPtrs) {
		delete [] meshBindingPtrs;
	}
	meshBindingPtrs = NULL;
	delete [] importedAssetBasename;
	importedAssetBasename = NULL;
	if (defaultAnimator) {
		delete defaultAnimator;
	}
	defaultAnimator = NULL;
}

void GLAssimpBinding::load(Context *context) {
	if (!importer) {
		BTAssimpImporter btImp;
		importer = btImp.importAsset(objAssetName);
		importedAssetBasename = strDuplicate(btImp.getImportedAssetBasename());
		scene = importer->GetScene();
		meshBindings = new StrHashTable<GLAssimpMeshBinding*>(scene->mNumMeshes * 1.5);
		meshBindingPtrs = new GLAssimpMeshBinding*[scene->mNumMeshes];
		for (U32 i = 0; i < scene->mNumMeshes; i++) {
			aiMesh *mesh = scene->mMeshes[i];
			GLAssimpMeshBinding *mb = new GLAssimpMeshBinding();
			mb->load(scene, mesh);
			meshBindings->put(mesh->mName.data, mb);
			meshBindingPtrs[i] = mb;
		}
		defaultAnimator = new AssimpAnimator();
		defaultAnimator->init(this, NULL);
	} else {
		// already have loaded once, check for GL reloads
		for (StrHashTable<GLAssimpMeshBinding*>::Iterator i = meshBindings->getIterator(); i.hasNext;) {
			GLAssimpMeshBinding *binding = meshBindings->getNext(i);
			if (!binding->faceIndicesVBOId || !binding->vertAttsVBOId) {
				binding->load(this->scene, binding->mesh);
			}
		}
	}
}

void GLAssimpBinding::unload(Context *context) {
	if (meshBindings) {
		for (StrHashTable<GLAssimpMeshBinding*>::Iterator i = meshBindings->getIterator(); i.hasNext;) {
			GLAssimpMeshBinding *binding = meshBindings->getNext(i);
			binding->unload(context);
		}
		meshBindings->deleteElements();
		meshBindings = NULL;
	}
	if (importer) {
		delete importer;
	}
	importer = NULL;
	scene = NULL;
	if (importedAssetBasename) {
		delete [] importedAssetBasename;
	}
	importedAssetBasename = NULL;
	if (defaultAnimator) {
		delete defaultAnimator;
	}
	defaultAnimator = NULL;
}

void GLAssimpBinding::invalidateGL() {
	if (meshBindings) {
		for (StrHashTable<GLAssimpMeshBinding*>::Iterator i = meshBindings->getIterator(); i.hasNext;) {
			GLAssimpMeshBinding *binding = meshBindings->getNext(i);
			binding->invalidateGL();
		}
	}
}

GLAssimpMeshBinding* GLAssimpBinding::getMeshBinding(const char* name) {
	return meshBindings->get(name);
}

}

#endif // BATTERYTECH_INCLUDE_ASSIMP


