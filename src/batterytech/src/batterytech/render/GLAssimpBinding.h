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
// Name        : GLAssimpBinding.h
// Description : Open Asset Importer to OpenGL VBO Bindings
//============================================================================

#ifndef GLASSIMPBINDING_H_
#define GLASSIMPBINDING_H_

#include "../batterytech_globals.h"
#include "../platform/platformgl.h"
#include "../util/HashTable.h"

#ifdef BATTERYTECH_INCLUDE_ASSIMP

#include "../../bt-assimp/include/aiScene.h"
#include "../../bt-assimp/include/assimp.hpp"

namespace BatteryTech {

class Context;
class GLAssimpMeshBinding;
class AssimpAnimator;

class GLAssimpBinding {
public:
	GLAssimpBinding(const char *objAssetName);
	virtual ~GLAssimpBinding();
	void load(Context *context);
	void unload(Context *context);
	void invalidateGL();
	GLAssimpMeshBinding* getMeshBinding(const char* name);
	char *objAssetName;
	Assimp::Importer *importer;
	const aiScene *scene;
	StrHashTable<GLAssimpMeshBinding*> *meshBindings;
	GLAssimpMeshBinding **meshBindingPtrs;
	char *importedAssetBasename;
	AssimpAnimator *defaultAnimator;
};

}

#endif

#endif /* GLASSIMPBINDING_H_ */
